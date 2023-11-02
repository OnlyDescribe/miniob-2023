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

  // 2. 创建表
  Session *session = sql_event->session_event()->session();
  rc = session->get_current_db()->create_table(view_name, attribute_count, select_attr_infos.data());
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }

  // 3. 将select的数据放入新表
  Table *new_table = session->get_current_db()->find_table(view_name);
  assert(new_table != nullptr);
  new_table->is_view() = true;  // 设置table属性, 为一视图表

  Record record;
  std::vector<Value> insert_values;
  insert_values.insert(insert_values.end(), values.begin(), values.end());
  rc = new_table->make_record(attribute_count, insert_values.data(), record);
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }
  rc = new_table->insert_record(record);
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }
  while (RC::SUCCESS == (rc = physical_operator->next())) {
    tuple = physical_operator->current_tuple();
    assert(tuple != nullptr);
    std::vector<Value> insert_values;
    for (int i = 0; i < attribute_count; i++) {
      rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        physical_operator->close();
        physical_operator.reset();
        return rc;
      }
      insert_values.push_back(value);
    }
    new_table->make_record(attribute_count, insert_values.data(), record);
    if (rc != RC::SUCCESS) {
      physical_operator->close();
      physical_operator.reset();
      return rc;
    }
    rc = new_table->insert_record(record);
    if (rc != RC::SUCCESS) {
      physical_operator->close();
      physical_operator.reset();
      return rc;
    }
  }

  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;
  } else {
    return rc;
  }

  rc = physical_operator->close();
  physical_operator.reset();

  return rc;
}