/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "common/log/log.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"

UpdateStmt::UpdateStmt(Table *table, const std::vector<const Value *> &values,
    const std::vector<const FieldMeta *> &field_metas, FilterStmt *filter_stmt)
    : table_(table), values_(values), field_metas_(field_metas), filter_stmt_(filter_stmt)
{}

RC UpdateStmt::create(Db *db, const UpdateSqlNode &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
    return RC::INVALID_ARGUMENT;
  }

  // 0. 找到对应的table
  Table *table = db->find_table(table_name);  // 只需要支持单表
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // 1. 处理更新字段
  std::vector<const Value *> values;
  std::vector<const FieldMeta *> field_metas;

  const int value_amount = update.assignments.size();

  const TableMeta &table_meta = table->table_meta();
  const int sys_field_num = table_meta.sys_field_num();
  const int field_num = table_meta.field_num();

  // 检查表中是否存在相应字段, 设置field_metas和values
  for (int i = 0; i < value_amount; ++i) {
    const std::string &attribute_name = update.assignments[i].attribute_name;

    if (attribute_name.empty()) {
      LOG_WARN("invalid argument");
      return RC::INVALID_ARGUMENT;
    }

    bool field_found{false};
    for (int j = sys_field_num; j < field_num; j++) {
      if (0 != strcmp(table_meta.field(j)->name(), attribute_name.c_str())) {
        continue;
      } else {
        field_found = true;
        field_metas.push_back(table_meta.field(j));
        break;
      }
    }

    // TODO(oldcb) 处理表达式: 子查询
    values.push_back(&update.assignments[i].value);  // 注意update资源的生命周期

    if (!field_found) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    // 检查字段和值类型是否冲突
    ASSERT(!field_metas.empty(), "");
    const AttrType value_type = update.assignments[i].value.attr_type();
    const AttrType field_type = field_metas.back()->type();
    if (field_type != value_type) {  // TODO try to convert the value type to field type
      LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
          table_name, field_metas.back()->name(), field_type, value_type);
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }
  }

  // 2. 处理过滤条件
  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));
  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;

  RC rc = FilterStmt::create(
      db, table, &table_map, update.conditions.data(), static_cast<int>(update.conditions.size()), filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // 3. everything alright
  stmt = new UpdateStmt(table, values, field_metas, filter_stmt);
  return RC::SUCCESS;
}
