
/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/operator/aggregation_physical_operator.h"
#include "sql/expr/expression.h"
#include "storage/table/table.h"
#include "event/sql_debug.h"

using namespace std;

// 聚合，需要支持表达式
RC AggregationPhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::INTERNAL;
  }
  // 投影表达式, 下面除了聚合，可能也有value
  // 或者纯聚合
  aggr_exprs_.clear();
  for (const auto& project: projects_) {
    AggretationExpr::get_aggrfuncexprs(project.get(), aggr_exprs_);
  }
  for (int i = 0; i < aggr_exprs_.size(); i++) {
    auto aggr_expr = static_cast<AggretationExpr*>(aggr_exprs_[i]);
    // 每个表达式都有值了
    aggr_expr->clear_value();
  }
  // TODO: 这里除了聚合表达式，还可以是字段表达式，值表达式
  ht_->init(aggr_exprs_);
  ht_->clear();
  const auto child_op = children_.front().get();
  RC rc = child_op->open(trx);
  
  AggregateKey key;
  while ((rc = child_op->next()) == RC::SUCCESS) {
    Tuple *tuple = child_op->current_tuple();
    ht_->insert_combine(key, *tuple);
  }
  is_execute_ = false;
  ht_->generate_aggregate_values();
  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;
  }

  return rc;
}

// 只能执行一次
RC AggregationPhysicalOperator::next()
{
  if (is_execute_) {
    return RC::RECORD_EOF;
  }
  AggregateKey key;
  const auto& results = ht_->get_aggr_value(key).aggregates_;
  assert(aggr_exprs_.size() == results.size());

  for (int i = 0; i < results.size(); i++) {
    auto aggr_expr = static_cast<AggretationExpr*>(aggr_exprs_[i]);
    // 每个表达式都有值了
    aggr_expr->set_value(results[i]);
  }
  
  aggregation_tuple_ = std::make_unique<AggregationTuple>(results);
  exprssion_tuple_.set_expressions(&projects_);
  exprssion_tuple_.set_tuple(aggregation_tuple_.get());
  is_execute_ = true;
  return RC::SUCCESS;
}

Tuple *AggregationPhysicalOperator::current_tuple() { return &exprssion_tuple_; }
