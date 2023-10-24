/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */


#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/stmt/join_on_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

JoinOnStmt::~JoinOnStmt()
{
  for (JoinOnUnit *unit : join_units_) {
    delete unit;
  }
  join_units_.clear();
}

RC JoinOnStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const ConditionSqlNode *conditions, int condition_num, JoinOnStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  JoinOnStmt *tmp_stmt = new JoinOnStmt();
  for (int i = 0; i < condition_num; i++) {
    JoinOnUnit *join_unit = nullptr;
    rc = create_join_unit(db, default_table, tables, conditions[i], join_unit);
    if (rc != RC::SUCCESS) {
      delete tmp_stmt;
      LOG_WARN("failed to create filter unit. condition index=%d", i);
      return rc;
    }
    tmp_stmt->join_units_.push_back(join_unit);
  }

  stmt = tmp_stmt;
  return rc;
}

RC JoinOnStmt::get_table_and_field(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const RelAttrSqlNode &attr, Table *&table, const FieldMeta *&field)
{
  if (common::is_blank(attr.relation_name.c_str())) {
    table = default_table;
  } else if (nullptr != tables) {
    auto iter = tables->find(attr.relation_name);
    if (iter != tables->end()) {
      table = iter->second;
    }
  } else {
    table = db->find_table(attr.relation_name.c_str());
  }
  if (nullptr == table) {
    LOG_WARN("No such table: attr.relation_name: %s", attr.relation_name.c_str());
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  field = table->table_meta().field(attr.attribute_name.c_str());
  if (nullptr == field) {
    LOG_WARN("no such field in table: table %s, field %s", table->name(), attr.attribute_name.c_str());
    table = nullptr;
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  return RC::SUCCESS;
}

RC JoinOnStmt::create_join_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const ConditionSqlNode &condition, JoinOnUnit *&join_unit)
{
  RC rc = RC::SUCCESS;

  CompOp comp = condition.comp;
  if (comp < EQUAL_TO || comp >= NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  join_unit = new JoinOnUnit;

  if (condition.left_is_attr) {
    Table *table = nullptr;
    const FieldMeta *field = nullptr;
    rc = get_table_and_field(db, default_table, tables, condition.left_attr, table, field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      return rc;
    }
    JoinOnObj filter_obj;
    filter_obj.init_attr(Field(table, field));
    join_unit->set_left(filter_obj);
  } else {
    JoinOnObj filter_obj;
    filter_obj.init_value(condition.left_value);
    join_unit->set_left(filter_obj);
  }

  if (condition.right_is_attr) {
    Table *table = nullptr;
    const FieldMeta *field = nullptr;
    rc = get_table_and_field(db, default_table, tables, condition.right_attr, table, field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      return rc;
    }
    JoinOnObj filter_obj;
    filter_obj.init_attr(Field(table, field));
    join_unit->set_right(filter_obj);
  } else {
    JoinOnObj filter_obj;
    filter_obj.init_value(condition.right_value);
    join_unit->set_right(filter_obj);
  }

  join_unit->set_comp(comp);

  // 检查两个类型是否能够比较
  return rc;
}