#include "sql/executor/create_table_select_executor.h"

#include "session/session.h"
#include "common/log/log.h"
#include "sql/parser/value.h"
#include "storage/table/table.h"
#include "sql/stmt/create_table_select_stmt.h"
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

RC CreateTableSelectExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  ASSERT(stmt->type() == StmtType::CREATE_TABLE_SELECT,
      "create table executor can not run this command: %d",
      static_cast<int>(stmt->type()));

  CreateTableSelectStmt *create_table_select_stmt = static_cast<CreateTableSelectStmt *>(stmt);

  const char *table_name = create_table_select_stmt->table_name().c_str();
  const std::vector<Table *> &select_tables = create_table_select_stmt->select_stmt()->tables();
  Session *session = sql_event->session_event()->session();

  // 因为经过handle_request_with_physical_operator, schema和物理算子都放进了sql_result
  SqlResult *sql_result = sql_event->session_event()->sql_result();
  Session *sql_session = sql_event->session_event()->session();

  // 1. 从select获取并设置新表的attr_infos
  std::vector<AttrInfoSqlNode> select_attr_infos;
  const TupleSchema &schema = sql_result->physical_operator()->tuple_schema();
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

  // 1. 收集所有的values以及一些信息
  Tuple *tuple;
  Value value;
  std::vector<std::vector<Value>> values_vec;  // 收集所有的values
  std::vector<int> nulls(attribute_count, 1);  // 为了确保type不为null， 从values获取各字段类型

  while (RC::SUCCESS == (rc = physical_operator->next())) {
    tuple = physical_operator->current_tuple();

    std::vector<Value> values;
    for (int i = 0; i < attribute_count; i++) {
      rc = tuple->cell_at(i, value);
      if (rc != RC::SUCCESS) {
        physical_operator->close();
        physical_operator.reset();
        return rc;
      }
      values.push_back(value);

      if (value.attr_type() != AttrType::NULLS) {
        nulls[i] = 0;
      }
      select_attr_infos[i].type = value.attr_type();
      select_attr_infos[i].length = value.length();
    }
    values_vec.push_back(std::move(values));
  }

  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;
  }

  // TODO: select_attr_infos设置是否可以为NULL, 参考view, null字段四则运算组合也是null

  // 设置表列名
  for (int i = 0; i < attribute_count; i++) {
    const TupleCellSpec &spec = schema.cell_at(i);
    std::string alias = spec.alias();
    if (!alias.empty()) {
      size_t dotPos = alias.find('.');
      if (dotPos != std::string::npos) {
        select_attr_infos[i].name = alias.substr(dotPos + 1);
      } else {
        select_attr_infos[i].name = alias;
      }
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

  //  比较Create table和Select的attr_infos, 并找出 table 比 select 多增加的字段
  std::vector<AttrInfoSqlNode> table_attr_infos = create_table_select_stmt->attr_infos();
  std::vector<int> difference_index;

  if (table_attr_infos.size() > 0) {
    auto attr_compare = [](const AttrInfoSqlNode &lhs, const AttrInfoSqlNode &rhs) {
      return lhs.name.compare(rhs.name);
    };

    std::sort(table_attr_infos.begin(), table_attr_infos.end(), attr_compare);
    auto last = std::unique(table_attr_infos.begin(),
        table_attr_infos.end(),
        [](const AttrInfoSqlNode &lhs, const AttrInfoSqlNode &rhs) { return lhs.name == rhs.name; });
    if (last != table_attr_infos.end()) {
      LOG_WARN("Duplicate column name");
      return RC::INVALID_ARGUMENT;
    }
    std::vector<AttrInfoSqlNode> select_attr_infos_copy;  // 不能打乱原来的select_attr_infos的顺序
    select_attr_infos_copy.reserve(select_attr_infos.size());
    std::copy(select_attr_infos.begin(),
        select_attr_infos.end(),
        std::back_inserter(select_attr_infos_copy));  // 不能打乱原来的select_attr_infos的顺序
    std::sort(select_attr_infos_copy.begin(), select_attr_infos_copy.end(), attr_compare);
    last = std::unique(select_attr_infos_copy.begin(),
        select_attr_infos_copy.end(),
        [](const AttrInfoSqlNode &lhs, const AttrInfoSqlNode &rhs) { return lhs.name == rhs.name; });
    if (last != select_attr_infos_copy.end()) {
      LOG_WARN("Duplicate column name");
      return RC::INVALID_ARGUMENT;
    }

    std::vector<int> table_index(table_attr_infos.size());
    std::vector<int> select_index(select_attr_infos_copy.size());
    std::iota(table_index.begin(), table_index.end(), 0);
    std::iota(select_index.begin(), select_index.end(), 0);

    // 用索引是因为要判断相同名的字段是否可以类型转换
    // TODO(oldcb): 判断不同类型的是否转换, 如 table_attr 是 float, select_attr 是int, 也是合法的
    std::vector<AttrInfoSqlNode> attr_infos_intersection;
    std::vector<int> intersect_index;  // table 和 select 重复字段
    std::set_intersection(table_index.begin(),
        table_index.end(),
        select_index.begin(),
        select_index.end(),
        std::back_inserter(intersect_index),
        [&](const auto &lhs, const auto &rhs) {
          return table_attr_infos[lhs].name < select_attr_infos_copy[rhs].name;
        });

    // 找出 table 比 select 多增加的字段
    std::vector<int> index(table_attr_infos.size());
    std::iota(index.begin(), index.end(), 0);
    std::set_difference(index.begin(),
        index.end(),
        intersect_index.begin(),
        intersect_index.end(),
        std::back_inserter(difference_index));
  }

  // 若从空表创建, 尝试从原表中获取信息
  if (values_vec.empty()) {
    for (int i = 0; i < attribute_count; i++) {
      for (int j = 0; j < select_tables.size(); ++j) {
        const TableMeta &select_table_meta =
            session->get_current_db()->find_table(select_tables[0]->name())->table_meta();
        if (select_table_meta.field(select_attr_infos[i].name.c_str()) != nullptr) {
          nulls[i] = 0;
          select_attr_infos[i].type = select_table_meta.field(select_attr_infos[i].name.c_str())->type();
          select_attr_infos[i].length = select_table_meta.field(select_attr_infos[i].name.c_str())->len();
          break;
        }
      }
    }
  }

  // 实在没办法, 找到NULL列, 替换为默认的INT
  for (int i = 0; i < nulls.size(); ++i) {
    if (nulls[i] == 1) {
      select_attr_infos[i].type = AttrType::INTS;
      select_attr_infos[i].length = 4;
    }
  }

  //  根据 table_attr_infos, 调整 attr_infos
  int diff_attr_num = difference_index.size();
  attribute_count += diff_attr_num;
  std::vector<AttrInfoSqlNode> attr_infos;
  attr_infos.reserve(difference_index.size() + select_attr_infos.size());
  for (int ind : difference_index) {
    attr_infos.push_back(table_attr_infos[ind]);
  }
  attr_infos.insert(attr_infos.end(), select_attr_infos.begin(), select_attr_infos.end());

  // 2. 创建表
  rc = session->get_current_db()->create_table(table_name, attribute_count, attr_infos.data());
  if (rc != RC::SUCCESS) {
    physical_operator->close();
    physical_operator.reset();
    return rc;
  }

  // 3. 将select的数据放入新表
  Table *new_table = session->get_current_db()->find_table(table_name);
  assert(new_table != nullptr);
  Record record;

  for (auto &&values : values_vec) {
    std::vector<Value> insert_values(diff_attr_num, Value(AttrType::NULLS));
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
  }

  rc = physical_operator->close();
  physical_operator.reset();

  return rc;
}