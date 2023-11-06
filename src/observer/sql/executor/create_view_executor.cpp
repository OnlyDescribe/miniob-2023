#include "sql/executor/create_view_executor.h"

#include "session/session.h"
#include "common/log/log.h"
#include "sql/parser/value.h"
#include "storage/table/table.h"
#include "sql/stmt/create_view_stmt.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "storage/db/db.h"
#include "storage/trx/trx.h"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <iterator>
#include <numeric>
#include <regex>
#include <utility>
#include <vector>

RC CreateViewExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  ASSERT(stmt->type() == StmtType::CREATE_TABLE_SELECT,
      "create table executor can not run this command: %d",
      static_cast<int>(stmt->type()));

  CreateViewStmt *create_view_stmt = static_cast<CreateViewStmt *>(stmt);

  const char *view_name = create_view_stmt->table_name().c_str();
  const std::vector<std::string> &view_alias = create_view_stmt->alias();

  // 因为经过handle_request_with_physical_operator, schema和物理算子都放进了sql_result
  SqlResult *sql_result = sql_event->session_event()->sql_result();
  Session *sql_session = sql_event->session_event()->session();

  // 1. 从select获取信息, 并设置新表的attr_infos
  std::vector<AttrInfoSqlNode> select_attr_infos;
  const TupleSchema &schema = sql_result->physical_operator()->tuple_schema();
  int attribute_count = schema.cell_num();
  select_attr_infos.resize(attribute_count);

  // 1.1 开启select
  Trx *trx = sql_session->current_trx();
  trx->start_if_need();
  std::unique_ptr<PhysicalOperator> &physical_operator = sql_result->physical_operator();

  RC rc = physical_operator->open(trx);
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }

  // 1.2 从values获取各字段属性, 以及字段所对应的原表(如果有的话)
  Session *session = sql_event->session_event()->session();

  bool view_is_modifiable = true;          // View是否可被修改
  std::vector<const Table *> view_tables;  // 在 Table 的元信息中设置每个字段对应的原表指针

  Tuple *tuple;
  Value value;

  std::vector<int> nulls(attribute_count, 1);  // 为了确保type不为null

  rc = physical_operator->next();
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }
  tuple = physical_operator->current_tuple();
  // assert(tuple != nullptr);
  for (int i = 0; i < attribute_count; i++) {
    RecordPos rid;
    rc = tuple->record_at(i, rid);
    if (rc == RC::NOTFOUND) {
      view_is_modifiable = false;
      view_tables.push_back(nullptr);
    } else {
      view_tables.push_back(session->get_current_db()->find_table(rid.table_id));
    }

    rc = tuple->cell_at(i, value);
    if (rc != RC::SUCCESS) {
      physical_operator->close();
      physical_operator.reset();
      return rc;
    }

    if (value.attr_type() != AttrType::NULLS) {
      nulls[i] = 0;
    }
    select_attr_infos[i].type = value.attr_type();
    select_attr_infos[i].length = value.length();
  }

  // 继续设置select_attr_infos[i].type
  while (std::accumulate(nulls.begin(), nulls.end(), 0) != 0) {
    while (RC::SUCCESS == (rc = physical_operator->next())) {
      tuple = physical_operator->current_tuple();
      assert(tuple != nullptr);
      for (int i = 0; i < attribute_count; i++) {
        rc = tuple->cell_at(i, value);
        if (rc != RC::SUCCESS) {
          physical_operator->close();
          physical_operator.reset();
          return rc;
        }
        if (nulls[i] == 1 && value.attr_type() != AttrType::NULLS) {
          nulls[i] = 0;
          select_attr_infos[i].type = value.attr_type();
        }
      }
    }
  }

  // 1.3 设置表列名
  for (int i = 0; i < attribute_count; i++) {
    const TupleCellSpec &spec = schema.cell_at(i);
    std::string alias = spec.alias();
    std::string field_name;
    size_t dotPos = alias.find('.');
    if (dotPos != std::string::npos) {
      field_name = alias.substr(dotPos + 1);
    } else {
      field_name = alias;
    }

    // 从alias中获取别名
    if (!view_alias.empty()) {
      if (view_alias.size() != attribute_count) {
        LOG_WARN("Duplicate column counts");
        return RC::INVALID_ARGUMENT;
      }
      select_attr_infos[i].name = view_alias[i];
    }  // 从schema中获取列别名
    else {
      if (alias.empty()) {
        LOG_WARN("Duplicate column counts");
        return RC::INVALID_ARGUMENT;
      } else {
        select_attr_infos[i].name = field_name;
      }
    }
    // 设置是否not null
    if (view_tables[i]) {
      select_attr_infos[i].is_not_null = view_tables[i]->table_meta().field(field_name.c_str())->is_not_null();
    }
  }

  // 如果列名重复, FAILURE
  std::set<std::string_view> unique_attrs;
  for (auto &&attr_info : select_attr_infos) {
    if (unique_attrs.count(attr_info.name) > 0) {
      LOG_WARN("Duplicate column name");
      return RC::INVALID_ARGUMENT;
    }
    unique_attrs.insert(attr_info.name);
  }

  // 2. 创建视图表 // TODO(oldcb): 在exit的时候销毁视图
  rc = session->get_current_db()->create_table(view_name, attribute_count, select_attr_infos.data());
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }

  // 3. 设置表关于视图的属性
  Table *view = session->get_current_db()->find_table(view_name);
  assert(view != nullptr);

  // 3.1 设置table属性, 为一视图表
  view->is_view() = true;
  view->view_db() = session->get_current_db();

  // 3.2 设置视图中"子计划的算子"
  std::regex pattern("AS\\s+([^\f\r\t\v]+)", std::regex_constants::icase);
  std::smatch match;
  if (std::regex_search(sql_event->sql(), match, pattern)) {
    view->view_sql() = match.str(1);
  } else {
    return RC::INTERNAL;
  }

  // 3.3.1 如果Select字段表达式涉及到的表大于1, 则不支持删增操作
  view->modifiable() = view_is_modifiable;
  int count{0};
  for (auto &&table : view_tables) {
    if (table == nullptr) {
      if (++count > 1) {
        view->modifiable() = false;
      }
    }
  }

  // 3.3.2 如果涉及到聚合或是GroupBy, 则不支持修改操作
  view->updatable() = view_is_modifiable;

  // 3.3.3 在 Table 的元信息中设置每个字段对应的原表指针
  view->set_view_tables(view_tables);

  // 3.3.4
  view->schema() = new TupleSchema(schema);

  view->alias() = view_alias;

  rc = physical_operator->close();
  physical_operator.reset();
  return rc;
}