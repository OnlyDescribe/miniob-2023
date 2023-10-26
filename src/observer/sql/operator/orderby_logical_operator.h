/*
 * @Author: zero-piB 
 * @Date: 2023-10-26 02:03:30
 * @LastEditors: zero-piB 13260648390@163.com
 * @LastEditTime: 2023-10-26 11:11:16
 * @FilePath: /miniob-2023/src/observer/sql/operator/orderby_logical_operator.h
 * 
 * Copyright (c) 2023 by ${git_name_email}, All Rights Reserved. 
 */
#pragma once

#include "sql/operator/logical_operator.h"
#include "sql/expr/expression.h"

/**
 * @brief orderby谓词/过滤逻辑算子
 * @ingroup LogicalOperator
 */
class OrderByLogicalOperator : public LogicalOperator
{
public:
  OrderByLogicalOperator(std::vector<std::unique_ptr<FieldExpr>> &&expressions, std::vector<SortType> &&sort_types) :
  sortTypes(std::move(sort_types)),
  field_expressions(std::move(expressions)) {}
  virtual ~OrderByLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::ORDER_BY; }

  std::vector<SortType> sortTypes;
  std::vector<std::unique_ptr<FieldExpr>> field_expressions;
};
