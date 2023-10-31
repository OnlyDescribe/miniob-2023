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
  for (const auto &units : join_units_) {
    for (auto unit : units) {
      delete unit;
    }
  }
  join_units_.clear();
}

RC JoinOnStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    std::vector<PConditionExpr *> conditions, JoinOnStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  JoinOnStmt *tmp_stmt = new JoinOnStmt();

  // TODO: PConditionExpr 包含了AND OR等表达式
  // 与 FilterStmt 类似

  // condition_num 个表
  for (int i = 0; i < conditions.size(); i++) {
    // 左表和右表的链接条件
    std::vector<JoinOnUnit *> join_units;

    // 创建当前表的链接条件
    auto create_stmt = [&](auto self, PExpr *cond, RC &rc) -> void {
      if (rc != RC::SUCCESS) {
        return;
      }
      if (cond->type == PExpType::COMPARISON) {
        if (cond && cond->cexp->comp != CompOp::AND) {  // 注意cond可能是空指针
          JoinOnUnit *join_unit = nullptr;
          rc = create_join_unit(db, default_table, tables, cond->cexp, join_unit);
          if (rc != RC::SUCCESS) {
            delete tmp_stmt;
            LOG_WARN("failed to create filter unit");
            return;
          }
          join_units.push_back(join_unit);
          return;
        }
        self(self, cond->cexp->left, rc);
        self(self, cond->cexp->right, rc);
      }
      return;
    };
    if (conditions[i] != nullptr)  // 注意conditions可能是空指针
    {
      PExpr pexp(conditions[i]);
      create_stmt(create_stmt, &pexp, rc);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to create filter unit");
        return rc;
      }
    }
    tmp_stmt->join_units_.push_back(join_units);
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
    PConditionExpr *condition, JoinOnUnit *&join_unit)
{
  RC rc = RC::SUCCESS;

  CompOp comp = condition->comp;
  if (comp < CompOp::EQUAL_TO || comp >= CompOp::NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  // TODO: 我这里默认左右都是UNARY表达式, 后续要把 FilterUnit 改成递归的送到 Expression
  join_unit = new JoinOnUnit;
  PUnaryExpr *left = condition->left->uexp;    // 暂时默认是UNARY表达式,
  PUnaryExpr *right = condition->right->uexp;  // 暂时默认是UNARY表达式,

  if (left->is_attr) {
    Table *table = nullptr;
    const FieldMeta *field = nullptr;
    rc = get_table_and_field(db, default_table, tables, left->attr, table, field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      return rc;
    }
    JoinOnObj filter_obj;
    filter_obj.init_attr(Field(table, field));
    join_unit->set_left(filter_obj);
  } else {
    JoinOnObj filter_obj;
    filter_obj.init_value(left->value);
    join_unit->set_left(filter_obj);
  }

  if (right->is_attr) {
    Table *table = nullptr;
    const FieldMeta *field = nullptr;
    rc = get_table_and_field(db, default_table, tables, right->attr, table, field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("cannot find attr");
      return rc;
    }
    JoinOnObj filter_obj;
    filter_obj.init_attr(Field(table, field));
    join_unit->set_right(filter_obj);
  } else {
    JoinOnObj filter_obj;
    filter_obj.init_value(right->value);
    join_unit->set_right(filter_obj);
  }

  join_unit->set_comp(comp);

  // 检查两个类型是否能够比较
  return rc;
}
