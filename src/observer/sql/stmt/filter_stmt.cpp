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

#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "storage/db/db.h"
#include "storage/table/table.h"
#include <cassert>

FilterStmt::~FilterStmt()
{
  for (FilterUnit *unit : filter_units_) {
    delete unit;
  }
  filter_units_.clear();
}

RC FilterStmt::create(Db *db, Table *default_table, const std::unordered_map<std::string, Table *> &parent_table_map,
    const std::unordered_map<std::string, Table *> & cur_table_map,
    PConditionExpr *conditions, FilterStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  FilterStmt *tmp_stmt = new FilterStmt();

  // 但目前仅默认是 AND, 即除了叶子节点都是些PConditionExpr, CompOp是AND的
  // 因为都只是 exp AND exp AND exp 的情况, 只要递归拿出exp, 创建filter_unit就行了, 瞎写的,
  // 或许还是直接把这棵树转成expression比较好。这里默认 OR AND 优先级最低, 只要找到不是 OR 和 AND 的 Expr 就行
  auto create_stmt = [&](auto self, PExpr *cond) -> RC {
    if (cond->type == PExpType::COMPARISON) {
      RC rc = RC::SUCCESS;
      auto comp = cond->cexp->comp;
      // TODO: PConditionExpr 包含了AND OR等表达式
      if (comp != CompOp::AND && comp != CompOp::OR) {
        if (comp < CompOp::EQUAL_TO || comp >= CompOp::NO_OP) {
          LOG_WARN("invalid compare operator : %d", comp);
          return RC::INVALID_ARGUMENT;
        }
        FilterUnit *filter_unit = new FilterUnit;
        // rc = create_filter_unit(db, default_table, tables, cond->cexp, filter_unit);
        Expression *left = nullptr;
        rc = Expression::create_expression(cond->cexp->left, parent_table_map, cur_table_map, left, cond->cexp->comp, db);
        if (rc != RC::SUCCESS) {
          delete tmp_stmt;
          LOG_WARN("failed to create filter unit");
          return rc;
        }
        filter_unit->left.reset(left);
        Expression *right = nullptr;
        rc = Expression::create_expression(cond->cexp->right, parent_table_map, cur_table_map, right, cond->cexp->comp, db);
        if (rc != RC::SUCCESS) {
          delete tmp_stmt;
          delete left;
          LOG_WARN("failed to create filter unit");
          return rc;
        }
        filter_unit->right.reset(right);
        filter_unit->comp = comp;
        tmp_stmt->filter_units_.push_back(filter_unit);
        return RC::SUCCESS;
      } else {
        tmp_stmt->is_or = comp == CompOp::OR;
      }
      rc = self(self, cond->cexp->left);
      rc = self(self, cond->cexp->right);
    }
    return rc;
  };
  if (conditions != nullptr)  // 注意conditions可能是空指针
  {
    PExpr pexp(conditions);
    rc = create_stmt(create_stmt, &pexp);
  }

  stmt = tmp_stmt;
  return rc;
}

RC get_table_and_field(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    const RelAttrSqlNode &attr, Table *&table, const FieldMeta *&field)
{
  if (common::is_blank(attr.relation_name.c_str())) {
    table = default_table;
  } else if (nullptr != tables) {
    auto iter = tables->find(attr.relation_name);
    if (iter != tables->end()) {
      table = iter->second;
    }
  }

  if (table == nullptr) {
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
