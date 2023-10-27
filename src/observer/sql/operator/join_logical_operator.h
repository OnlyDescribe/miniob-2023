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
// Created by Wangyunlai on 2022/12/07
//

#pragma once
#include <memory>

#include "sql/expr/expression.h"
#include "sql/operator/logical_operator.h"

/**
 * @brief 连接算子
 * @ingroup LogicalOperator
 * @details 连接算子，用于连接两个表。对应的物理算子或者实现，可能有NestedLoopJoin，HashJoin等等。
 */
class JoinLogicalOperator : public LogicalOperator
{
public:
  JoinLogicalOperator() = default;

  virtual ~JoinLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::JOIN; }

  void add_expression(std::unique_ptr<Expression> &&expression) {
    expressions_.emplace_back(std::move(expression));
  }

  bool has_equal_cmp_expression() {
    for (const auto& expression: expressions_) {
      if (expression->type() == ExprType::COMPARISON) {
        auto exp = static_cast<ComparisonExpr*>(expression.get());
        if (exp->comp() == CompOp::EQUAL_TO) {
          return true;
        }
      }
    }
    return false;
  }

  // 右表，用于实现on 时的 过滤谓词下推
  void set_right_table(Table* table) { right_table_ = table; }

private:
  LogicalOperatorType type_{LogicalOperatorType::JOIN};
  int equal_expes_cnt{0};                     
  Table* right_table_{nullptr};
};
