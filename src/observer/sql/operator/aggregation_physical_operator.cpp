
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
#include "storage/table/table.h"
#include "event/sql_debug.h"

using namespace std;

RC AggregationPhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::INTERNAL;
  }
  aggr_results_ = ht_->generate_initial_aggregateValue();
  ht_->clear();
  not_null_record_num_ = ht_->generate_initial_aggregateValue();
  const auto child_op = children_.front().get();
  RC rc = child_op->open(trx);
  Tuple *tuple;
  while (child_op->next() == RC::SUCCESS) {
    tuple = child_op->current_tuple();

    // 从列中获取聚合表达式的值
    AggregateValue aggr_value;
    aggr_value.aggregates_.reserve(aggr_exprs_.size());
    for (int i = 0; i < aggr_exprs_.size(); i++) {
      Value value;
      aggr_exprs_[i]->get_value(*tuple, value);
      aggr_value.aggregates_.emplace_back(value);
    }
    ht_->combine_aggregate_values(&aggr_results_, aggr_value, not_null_record_num_);
  }
  is_execute_ = false;
  return rc;
}

// 只能执行一次
RC AggregationPhysicalOperator::next()
{
  if (is_execute_) {
    return RC::RECORD_EOF;
  }
  // 拿到所有的fields
  std::vector<Field> fields;
  for (const auto &aggr_expr : aggr_exprs_) {
    fields.push_back(static_cast<AggretationExpr *>(aggr_expr.get())->filed());
  }
  // 更改avg的结果, 统一改成float
  for (int i = 0; i < aggr_results_.aggregates_.size(); i++) {
    if (!not_null_record_num_.aggregates_[i].is_null() &&
        static_cast<AggretationExpr *>(aggr_exprs_[i].get())->aggr_type() == AggrFuncType::AVG) {
      aggr_results_.aggregates_[i] =
          Value(aggr_results_.aggregates_[i].get_float() / not_null_record_num_.aggregates_[i].get_int());
    }
  }
  tuple_ = std::make_unique<AggregationTuple>(aggr_results_.aggregates_, fields);
  is_execute_ = true;
  return RC::SUCCESS;
}

Tuple *AggregationPhysicalOperator::current_tuple() { return tuple_.get(); }
