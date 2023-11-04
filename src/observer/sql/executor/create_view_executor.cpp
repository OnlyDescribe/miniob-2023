#include "sql/executor/create_view_executor.h"

#include "session/session.h"
#include "common/log/log.h"
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
#include <vector>

RC CreateViewExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  ASSERT(stmt->type() == StmtType::CREATE_TABLE_SELECT,
      "create table executor can not run this command: %d",
      static_cast<int>(stmt->type()));

  CreateViewStmt *create_view_stmt = static_cast<CreateViewStmt *>(stmt);

  const char *view_name = create_view_stmt->table_name().c_str();

  // 因为经过handle_request_with_physical_operator, schema和物理算子都放进了sql_result
  SqlResult *sql_result = sql_event->session_event()->sql_result();
  Session *sql_session = sql_event->session_event()->session();

  // 1. 从select获取并设置新表的attr_infos
  std::vector<AttrInfoSqlNode> select_attr_infos;
  const TupleSchema &schema = sql_result->tuple_schema();
  int attribute_count = schema.cell_num();
  select_attr_infos.resize(attribute_count);

  // 开启select
  Trx *trx = sql_session->current_trx();
  trx->start_if_need();
  std::unique_ptr<PhysicalOperator> &physical_operator = sql_result->physical_operator();

  RC rc = physical_operator->open(trx);
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }

  Tuple *tuple;
  Value value;
  std::vector<Value> values;

  // 从values获取各字段类型
  rc = physical_operator->next();
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }
  tuple = physical_operator->current_tuple();
  assert(tuple != nullptr);
  for (int i = 0; i < attribute_count; i++) {
    rc = tuple->cell_at(i, value);
    if (rc != RC::SUCCESS) {
      physical_operator->close();
      physical_operator.reset();
      return rc;
    }
    values.push_back(value);
    select_attr_infos[i].type = value.attr_type();
    select_attr_infos[i].length = value.length();
  }

  // 设置表列名
  for (int i = 0; i < attribute_count; i++) {
    const TupleCellSpec &spec = schema.cell_at(i);
    const char *alias = spec.alias();
    if (nullptr != alias || alias[0] != 0) {
      select_attr_infos[i].name = alias;
    }
  }

  // 2. 创建表 // TODO(oldcb): 在exit的时候销毁视图
  Session *session = sql_event->session_event()->session();
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

  // 3.2 设置视图中子计划的逻辑算子
  view->logical_operator() = sql_event->logical_operator().release();

  std::unique_ptr<SelectStmt> &select_stmt = create_view_stmt->select_stmt();
  const std::vector<Field> &query_fields = select_stmt->query_fields();
  // 3.3.1 如果Select字段表达式涉及到的表大于1, 则不支持增删操作
  view->modifiable() = true;
  for (int i = 1, j = 0; i < query_fields.size(); ++i, ++j) {
    if (query_fields[i].table_name() != nullptr && query_fields[j].table_name() != nullptr) {
      if (strcmp(query_fields[i].table_name(), query_fields[j].table_name()) != 0) {
        view->modifiable() = false;
      }
    }
  }
  // TODO(oldcb): // 3.3.2 如果涉及到聚合或是GroupBy, 则不支持修改操作
  view->updatable() = true;

  // 3.4 在 Table 的元信息中设置每个字段对应的原表指针
  std::vector<const Table *> view_tables;
  view_tables.reserve(query_fields.size());
  for (auto &&query_field : query_fields) {
    view_tables.push_back(query_field.table());
  }
  view->set_view_tables(view_tables);

  rc = physical_operator->close();
  physical_operator.reset();
  return rc;
}