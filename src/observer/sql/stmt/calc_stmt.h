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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>
#include <memory>

#include "common/rc.h"
#include "sql/stmt/stmt.h"
#include "sql/expr/expression.h"

class Db;
class Table;

/**
 * @brief 描述算术运算语句
 * @ingroup Statement
 */
class CalcStmt : public Stmt
{
public:
  CalcStmt() = default;
  virtual ~CalcStmt() override = default;

  StmtType type() const override { return StmtType::CALC; }

public:
  static RC create(ParsedSqlNode &sqlnode, Stmt *&stmt)
  {
    CalcStmt *calc_stmt = new CalcStmt();
    if (sqlnode.calc.expressions.size()) {
      for (Expression *const expr : sqlnode.calc.expressions) {
        calc_stmt->expressions_.emplace_back(expr);
      }
      sqlnode.calc.expressions.clear();
    } else if (!sqlnode.selection.attributes.empty()) {
      for (auto& expr: sqlnode.selection.attributes) {
        Expression* tmp_expr;
        RC rc = Expression::create_expression(expr, {}, {}, tmp_expr);
        if (rc != RC::SUCCESS) {
          return rc;
        }
        calc_stmt->expressions_.emplace_back(tmp_expr);
        if (expr->alias.empty()) {
          tmp_expr->set_name(expr->name);
        } else {
          tmp_expr->set_name(expr->alias);
        }
      }
    }
    stmt = calc_stmt;
    return RC::SUCCESS;
  }

public:
  std::vector<std::unique_ptr<Expression>> &expressions() { return expressions_; }

private:
  std::vector<std::unique_ptr<Expression>> expressions_;
};
