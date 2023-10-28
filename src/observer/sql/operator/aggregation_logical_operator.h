/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include "sql/operator/logical_operator.h"
#include "sql/expr/expression.h"
#include "storage/field/field.h"

/**
 * @brief 表示聚合操作的算子，需要聚合表达式，以及多个字段的聚合。
 * @ingroup AggregationLogicalOperator
 */
class AggregationLogicalOperator : public LogicalOperator
{
public:
  AggregationLogicalOperator() {}
  virtual ~AggregationLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::AGGREGATION; }

  // 从fields 构造聚合表达式
  void set_aggregation_expr(const std::vector<Field> &fields)
  {
    for (const auto &field : fields) {
      expressions_.emplace_back(std::make_unique<AggretationExpr>(field, field.get_aggr_type()));
    }
  }
  void set_aggregation_expr(std::vector<std::unique_ptr<Expression>> &&exprs) { expressions_ = std::move(exprs); }
  const std::vector<std::unique_ptr<Expression>> &aggregation_expr() const { return expressions_; }
};
