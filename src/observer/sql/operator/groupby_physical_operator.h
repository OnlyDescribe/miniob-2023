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

#include <unordered_map>

#include "common/log/log.h"
#include "sql/operator/table_scan_physical_operator.h"
#include "sql/operator/aggregation_physical_operator.h"
#include "storage/record/record_manager.h"
#include "common/rc.h"

class Table;
class FilterStmt;

/**
 * @brief 聚合算子, 下边一般挂一个全表扫描的算子
 * @ingroup PhysicalOperator
 */
class GroupbyPhysicalOperator : public PhysicalOperator
{

public:
  GroupbyPhysicalOperator(std::vector<std::unique_ptr<Expression>> &&projcets, FilterStmt* having, 
    std::vector<std::unique_ptr<Expression>> &&groupbys)
      : projcets_(std::move(projcets)),
        having_(having),
        groupbys_(std::move(groupbys)){}

  // TODO: having_expr_会内存泄漏，但是如果加上delete后，这里有一个core
  virtual ~GroupbyPhysicalOperator() {
    // if (having_expr_ != nullptr) {
    //   delete having_expr_;
    // }
  }

  PhysicalOperatorType type() const override { return PhysicalOperatorType::GROUPBY; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;
  Tuple *current_tuple() override;
private:
  RC predicate(const Tuple& tuple, bool& res);
  RC create_having_expression();          // 将having语句处理成conjunction expression

private:
  std::vector<std::unique_ptr<Expression>> projcets_;  // 聚合表达式
  std::vector<std::unique_ptr<Expression>> groupbys_;    // groupby 子句, 应该是FieldExpr
  FilterStmt* having_;                                   // having, not own this;
  std::unordered_map<AggregateKey, SimpleAggregationHashTable> ht_;    // grouby + 聚合哈希表
  std::unordered_map<AggregateKey, SimpleAggregationHashTable>::iterator it_;
  AggregateValue aggr_results_;
  AggregateValue not_null_record_num_;
  std::unique_ptr<AggregationTuple> tuple_;
  std::vector<int> relations_;                // 字段映射

  // std::shared_ptr<ConjunctionExpr> having_expr_;
  ConjunctionExpr* having_expr_{nullptr};   // 用于having的过滤
};
