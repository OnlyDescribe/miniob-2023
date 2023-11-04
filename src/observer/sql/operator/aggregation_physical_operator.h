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
#include "storage/record/record_manager.h"
#include "common/rc.h"

class Table;

using ExpressionRef = unique_ptr<Expression>;

/** AggregateKey represents a key in an aggregation operation */
struct AggregateKey
{
  /** The group-by values */
  std::vector<Value> group_bys_;

  /**
   * Compares two aggregate keys for equality.
   * @param other the other aggregate key to be compared with
   * @return `true` if both aggregate keys have equivalent group-by expressions, `false` otherwise
   */
  auto operator==(const AggregateKey &other) const -> bool
  {
    for (uint32_t i = 0; i < other.group_bys_.size(); i++) {
      if (group_bys_[i].is_null() && other.group_bys_[i].is_null()) {
        return true;
      }
      if (group_bys_[i].is_null() || other.group_bys_[i].is_null()) {
        return false;
      }
      if (group_bys_[i].compare(other.group_bys_[i]) != 0) {
        return false;
      }
    }
    return true;
  }
};

/** Implements std::hash on AggregateKey */

namespace std {
template <>
struct hash<AggregateKey>
{
  auto operator()(const AggregateKey &agg_key) const -> std::size_t
  {
    std::size_t hash = 0;
    for (const Value &value : agg_key.group_bys_) {
      std::size_t valueHash = 0;

      // 根据 Value 对象的类型，计算哈希值
      switch (value.attr_type()) {
        case AttrType::INTS: valueHash = std::hash<int>{}(value.get_int()); break;
        case AttrType::FLOATS: valueHash = std::hash<float>{}(value.get_float()); break;
        case AttrType::BOOLEANS: valueHash = std::hash<bool>{}(value.get_boolean());  break;
        case AttrType::CHARS: valueHash = std::hash<std::string>{}(value.get_string()); break;
        case AttrType::NULLS: valueHash = std::hash<int>{}(-182301); break;      // null就随缘大法
        default: LOG_WARN("Unsupported field type"); break;
      }
      // 组合哈希值
      hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

}  // namespace std

/** AggregateValue represents a value for each of the running aggregates */
struct AggregateValue
{
  /** The aggregate values */
  std::vector<Value> aggregates_;
};

class SimpleAggregationHashTable
{

public:
  /**
   * Construct a new SimpleAggregationHashTable instance.
   * @param agg_exprs the aggregation expressions
   * @param agg_types the types of aggregations
   */
  SimpleAggregationHashTable(const std::vector<ExpressionRef> *agg_exprs) : agg_exprs_(agg_exprs) {}

  /**
   *
   * Combines the input into the aggregation result.
   * @param input The input value
   */
  void combine_aggregate_values(const AggregateValue &input)
  {
    assert(results_.aggregates_.size() == input.aggregates_.size());
    // 如果需要，先初始化
    auto init_value = [](const Value &intial_value, Value &output) -> bool {
      if (output.attr_type() == AttrType::NULLS) {
        output = intial_value;
        return true;
      }
      return false;
    };
    for (uint32_t i = 0; i < agg_exprs_->size(); i++) {
      auto aggr_expr = static_cast<AggretationExpr *>((*agg_exprs_)[i].get());
      switch (aggr_expr->aggr_type()) {
        case AggrFuncType::COUNT_STAR: results_.aggregates_[i] = Value::add(results_.aggregates_[i], Value(1)); break;
        case AggrFuncType::COUNT:
          if (!input.aggregates_[i].is_null()) {
            results_.aggregates_[i] = Value::add(results_.aggregates_[i], Value(1));
          }
          break;
        case AggrFuncType::AVG:
          if (!input.aggregates_[i].is_null()) {
            if (results_.aggregates_[i].is_null()) {
              results_.aggregates_[i] = input.aggregates_[i];
              null_not_record_.aggregates_[i] = Value(1);
            } else {
              results_.aggregates_[i] = Value::add(results_.aggregates_[i], input.aggregates_[i]);
              null_not_record_.aggregates_[i] = Value::add(null_not_record_.aggregates_[i], Value(1));
            }
          }
          break;
        case AggrFuncType::SUM:
          if (!input.aggregates_[i].is_null() && !init_value(input.aggregates_[i], results_.aggregates_[i])) {
            results_.aggregates_[i] = Value::add(results_.aggregates_[i], input.aggregates_[i]);
          }
          break;
        case AggrFuncType::MIN:
          if (!input.aggregates_[i].is_null() && !init_value(input.aggregates_[i], results_.aggregates_[i])) {
            if (input.aggregates_[i].compare(results_.aggregates_[i]) < 0) {
              results_.aggregates_[i] = input.aggregates_[i];
            }
          }
          break;
        case AggrFuncType::MAX:
          if (!input.aggregates_[i].is_null() && !init_value(input.aggregates_[i], results_.aggregates_[i])) {
            if (input.aggregates_[i].compare(results_.aggregates_[i]) > 0) {
              results_.aggregates_[i] = input.aggregates_[i];
            }
          }
          break;
        default: LOG_ERROR("aggretion invalid"); break;
      }
    }
  }

  void init() {
    null_not_record_ = generate_initial_aggregateValue();
    results_ = generate_initial_aggregateValue();
  }
  /**
   * @description: 因为平均数只是和，需要计算最终的结果
   * @return {*}
   */  
  void generate_aggregate_values() {
    // 更改avg的结果, 统一改成float
    for (int i = 0; i < results_.aggregates_.size(); i++) {
      if (!null_not_record_.aggregates_[i].is_null() &&
          static_cast<AggretationExpr *>((*agg_exprs_)[i].get())->aggr_type() == AggrFuncType::AVG) {
        results_.aggregates_[i] =
            Value(results_.aggregates_[i].get_float() / null_not_record_.aggregates_[i].get_int());
      }
    }
  }

  void clear() { ht_.clear(); }

  AggregateValue& aggr_results() {  return results_; }

private:
  /** @return The initial aggregate value for this aggregation executor */
  auto generate_initial_aggregateValue() -> AggregateValue
  {
    std::vector<Value> values{};
    for (const auto &agg_expr_ptr : *agg_exprs_) {
      auto aggr_expr = static_cast<AggretationExpr *>(agg_expr_ptr.get());
      switch (aggr_expr->aggr_type()) {
        case AggrFuncType::COUNT_STAR:
        case AggrFuncType::COUNT:
          // Count start starts at zero.
          values.emplace_back(Value(0));
          break;
        case AggrFuncType::MIN:
        case AggrFuncType::MAX:
        case AggrFuncType::SUM:
        case AggrFuncType::AVG:
          // other start starts at NULL.
          values.emplace_back(Value(AttrType::NULLS));
          break;
        default: break;
      }
    }
    return {values};
  }
  /** The hash table is just a map from aggregate keys to aggregate values */
  std::unordered_map<AggregateKey, AggregateValue> ht_{};
  const std::vector<ExpressionRef> *agg_exprs_{nullptr};  // not own this
  AggregateValue null_not_record_;
  AggregateValue results_;
};

/**
 * @brief 聚合算子, 下边一般挂一个全表扫描的算子
 * @ingroup PhysicalOperator
 */
class AggregationPhysicalOperator : public PhysicalOperator
{

public:
  AggregationPhysicalOperator(std::vector<std::unique_ptr<Expression>> &&aggr_exprs)
      : aggr_exprs_(std::move(aggr_exprs))
  {
    ht_ = std::make_unique<SimpleAggregationHashTable>(&aggr_exprs_);
  }

  virtual ~AggregationPhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::AGGREGATION; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override { return RC::SUCCESS; }
  Tuple *current_tuple() override;

  virtual TupleSchema tuple_schema() const override {
    TupleSchema schema;
    for (const auto& expr: aggr_exprs_) {
      schema.append_cell(expr->name().c_str());
    }
    return schema;
  }

private:
  std::vector<std::unique_ptr<Expression>> aggr_exprs_;  // 聚合表达式
  std::unique_ptr<SimpleAggregationHashTable> ht_;       // 聚合哈希表
  std::unique_ptr<AggregationTuple> tuple_;
  bool is_execute_{false};
};
