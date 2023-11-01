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

RC FilterStmt::create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    PConditionExpr *conditions, FilterStmt *&stmt)
{
  RC rc = RC::SUCCESS;
  stmt = nullptr;

  FilterStmt *tmp_stmt = new FilterStmt();

  // TODO: PConditionExpr 包含了AND OR等表达式
  // 但目前仅默认是 AND, 即除了叶子节点都是些PConditionExpr, CompOp是AND的
  // 因为都只是 exp AND exp AND exp 的情况, 只要递归拿出exp, 创建filter_unit就行了, 瞎写的,
  // 或许还是直接把这棵树转成expression比较好。这里默认 OR AND 优先级最低, 只要找到不是 OR 和 AND 的 Expr 就行
  auto create_stmt = [&](auto self, PExpr *cond) -> RC {
    if (cond->type == PExpType::COMPARISON) {
      RC rc = RC::SUCCESS;
      if (cond->cexp->comp != CompOp::AND && cond->cexp->comp != CompOp::OR) {
        FilterUnit *filter_unit = nullptr;
        rc = create_filter_unit(db, default_table, tables, cond->cexp, filter_unit);
        if (rc != RC::SUCCESS) {
          delete tmp_stmt;
          LOG_WARN("failed to create filter unit");
          return rc;
        }
        tmp_stmt->filter_units_.push_back(filter_unit);
        return RC::SUCCESS;
      } else {
        tmp_stmt->is_or = cond->cexp->comp == CompOp::OR;
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

RC FilterStmt::create_filter_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
    PConditionExpr *condition, FilterUnit *&filter_unit)
{
  RC rc = RC::SUCCESS;

  CompOp comp = condition->comp;
  if (comp < CompOp::EQUAL_TO || comp >= CompOp::NO_OP) {
    LOG_WARN("invalid compare operator : %d", comp);
    return RC::INVALID_ARGUMENT;
  }

  // TODO: 我这里默认左右都是UNARY表达式, 后续要把 FilterUnit 改成递归的送到 Expression
  filter_unit = new FilterUnit;
  filter_unit->comp = comp;

  if (condition->left->type == PExpType::UNARY) {
    PUnaryExpr *left = condition->left->uexp;
    if (left->is_attr) {
      Table *table = nullptr;
      const FieldMeta *field = nullptr;
      rc = get_table_and_field(db, default_table, tables, left->attr, table, field);
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot find attr");
        return rc;
      }
      filter_unit->left = std::make_unique<FieldExpr>(Field(table, field));
    } else {
      filter_unit->left = std::make_unique<ValueExpr>(left->value);
    }
  } else if (condition->left->type == PExpType::SUBQUERY) {
    Stmt *subquery_stmt = nullptr;
    rc = SelectStmt::create(db, *condition->left->sexp->sub_select, subquery_stmt);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    auto subquery_expr = std::make_unique<SubQueryExpr>();
    subquery_expr->subquery_stmt = static_cast<SelectStmt *>(subquery_stmt);
    filter_unit->left = std::move(subquery_expr);
  } else {
    LOG_ERROR("not support expr type");
    return RC::INVALID_ARGUMENT;
  }

  if (condition->right->type == PExpType::UNARY) {
    PUnaryExpr *right = condition->right->uexp;
    if (right->is_attr) {
      Table *table = nullptr;
      const FieldMeta *field = nullptr;
      rc = get_table_and_field(db, default_table, tables, right->attr, table, field);
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot find attr");
        return rc;
      }
      filter_unit->right = std::make_unique<FieldExpr>(Field(table, field));
    } else {
      filter_unit->right = std::make_unique<ValueExpr>(right->value);
    }
  } else if (condition->right->type == PExpType::SUBQUERY) {  // 子查询表达式
    Stmt *subquery_stmt = nullptr;
    rc = SelectStmt::create(db, *condition->right->sexp->sub_select, subquery_stmt);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    auto subquery_expr = std::make_unique<SubQueryExpr>();
    subquery_expr->subquery_stmt = static_cast<SelectStmt *>(subquery_stmt);
    filter_unit->right = std::move(subquery_expr);
  } else if (condition->right->type == PExpType::LIST) {
    PListExpr* right = condition->right->lexp;
    auto list_expr = std::make_unique<ListExpr>();
    list_expr->set_values(right->value_list);
    filter_unit->right = std::move(list_expr);
  } else {
    LOG_ERROR("not support expr type");
    return RC::INVALID_ARGUMENT;
  }
  // 检查两个类型是否能够比较
  return rc;
}
