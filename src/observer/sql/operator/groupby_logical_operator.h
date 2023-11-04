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
#include "sql/stmt/filter_stmt.h"

/**
 * @brief 表示聚合操作的算子，需要聚合表达式，以及多个字段的聚合。
 * @ingroup GroupbyLogicalOperator
 */
class GroupbyLogicalOperator : public LogicalOperator
{
public:
  // 使用聚合的field，以及分组的key
  GroupbyLogicalOperator(std::vector<std::unique_ptr<Expression>> &&o_groupbys, FilterStmt* o_having, const std::vector<Field> &o_query_fields):
    query_fields(o_query_fields),
    groupbys(std::move(o_groupbys)),
    having(o_having) {}
  virtual ~GroupbyLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::GROUPBY; }

  void set_aggregation_expr(std::vector<std::unique_ptr<Expression>> &&exprs) { expressions_ = std::move(exprs); }
  const std::vector<std::unique_ptr<Expression>> &aggregation_expr() const { return expressions_; }

  std::vector<Field> query_fields;
  std::vector<std::unique_ptr<Expression>> groupbys;      // groupby Exprs;

  FilterStmt* having{nullptr};      // not own this
};
