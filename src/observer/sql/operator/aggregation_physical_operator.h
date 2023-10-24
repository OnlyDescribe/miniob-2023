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
struct AggregateKey {
  /** The group-by values */
  std::vector<Value> group_bys_;

  /**
   * Compares two aggregate keys for equality.
   * @param other the other aggregate key to be compared with
   * @return `true` if both aggregate keys have equivalent group-by expressions, `false` otherwise
   */
  auto operator==(const AggregateKey &other) const -> bool {
    for (uint32_t i = 0; i < other.group_bys_.size(); i++) {
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
struct hash<AggregateKey> {
  auto operator()(const AggregateKey &agg_key) const -> std::size_t {
    std::size_t hash = 0;
    for (const Value& value : agg_key.group_bys_) {
        std::size_t valueHash = 0;

        // 根据 Value 对象的类型，计算哈希值
        switch (value.attr_type())
        {
        case AttrType::INTS:
          valueHash = std::hash<int>{}(value.get_int());
          break;
        case AttrType::FLOATS:
          valueHash = std::hash<float>{}(value.get_float());
        case AttrType::BOOLEANS:
          valueHash = std::hash<bool>{}(value.get_boolean());
        default:
          LOG_WARN("Unsupported field type");
          break;
        }
        // 组合哈希值
        hash ^= valueHash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

}

/** AggregateValue represents a value for each of the running aggregates */
struct AggregateValue {
  /** The aggregate values */
  std::vector<Value> aggregates_;
};


class SimpleAggregationHashTable {

public:
  /**
   * Construct a new SimpleAggregationHashTable instance.
   * @param agg_exprs the aggregation expressions
   * @param agg_types the types of aggregations
   */
  SimpleAggregationHashTable(const std::vector<ExpressionRef>* agg_exprs)
      : agg_exprs_(agg_exprs) {}

  /** @return The initial aggregate value for this aggregation executor */
  auto generate_initial_aggregateValue() -> AggregateValue {
    std::vector<Value> values{};
    for (const auto &agg_expr_ptr : *agg_exprs_) {
      auto aggr_expr = static_cast<AggretationExpr*>(agg_expr_ptr.get());
      switch (aggr_expr->aggr_type()) {
        case AggrFuncType::COUNT_STAR:
          // TODO: Others starts at null. 
          // Count start starts at zero.
          // ValueFactory::GetNullValueByType(TypeId::INTEGER)
          values.emplace_back(Value(0));
          break;
        case AggrFuncType::COUNT:
        case AggrFuncType::MIN:
        case AggrFuncType::MAX:
        case AggrFuncType::SUM:
        case AggrFuncType::AVG:
          values.emplace_back();
          // if (aggr_expr->value_type() == AttrType::FLOATS) {
          //   values.emplace_back(Value(static_cast<float>(0.0)));
          // } else {
          //   values.emplace_back(Value(0));
          // }
          break;
        default:
          break;
      }
    }
    return {values};
  }

  /**
   *
   * Combines the input into the aggregation result.
   * @param[out] result The output aggregate value
   * @param input The input value
   */
  void combine_aggregate_values(AggregateValue *result, const AggregateValue &input) {

    // 如果需要，先初始化
    auto init_value = [](const Value& intial_value, Value& output) -> bool {
      if (output.attr_type() == AttrType::UNDEFINED) {
        output = intial_value;
        return true;
      }
      return false;
    };
    for (uint32_t i = 0; i < agg_exprs_->size(); i++) {
      auto aggr_expr = static_cast<AggretationExpr*>((*agg_exprs_)[i].get());
      switch (aggr_expr->aggr_type()) {
        case AggrFuncType::COUNT_STAR:
        case AggrFuncType::COUNT:
          if (!init_value(Value(1), result->aggregates_[i])) {
            result->aggregates_[i] = Value::add(result->aggregates_[i], Value(1));
          }
          break;
        case AggrFuncType::AVG:
        case AggrFuncType::SUM:
          if (!init_value(input.aggregates_[i], result->aggregates_[i])) {
            result->aggregates_[i] = Value::add(result->aggregates_[i], input.aggregates_[i]);
          }
          break;
        case AggrFuncType::MIN:
          if (!init_value(input.aggregates_[i], result->aggregates_[i])) {
            if (input.aggregates_[i].compare(result->aggregates_[i]) < 0) {
              result->aggregates_[i] = input.aggregates_[i];
            }
          } 
          break;
        case AggrFuncType::MAX:
          if (!init_value(input.aggregates_[i], result->aggregates_[i])) {
            if (input.aggregates_[i].compare(result->aggregates_[i]) > 0) {
              result->aggregates_[i] = input.aggregates_[i];
            }
          }
          break;
        default:
          LOG_ERROR("aggretion invalid");
          break;
      }
    }
  }

  /**
   * Inserts a value into the hash table and then combines it with the current aggregation.
   * @param agg_key the key to be inserted
   * @param agg_val the value to be inserted
   */
  void insert_combine(const AggregateKey &agg_key, const AggregateValue &agg_val) {
    if (ht_.count(agg_key) == 0) {
      ht_.insert({agg_key, generate_initial_aggregateValue()});
    }
    combine_aggregate_values(&ht_[agg_key], agg_val);
  }

  void clear() { ht_.clear(); }

 private:
  /** The hash table is just a map from aggregate keys to aggregate values */
  std::unordered_map<AggregateKey, AggregateValue> ht_{};
  const std::vector<ExpressionRef> *agg_exprs_{nullptr};    // 本身不拥有表达式
};

/**
 * @brief 聚合算子, 下边一般挂一个全表扫描的算子
 * @ingroup PhysicalOperator
 */
class AggregationPhysicalOperator : public PhysicalOperator
{

public:
  AggregationPhysicalOperator(std::vector<std::unique_ptr<Expression>>&& aggr_exprs): aggr_exprs_(std::move(aggr_exprs)) {
    ht_ = std::make_unique<SimpleAggregationHashTable>(&aggr_exprs_);
  }

  virtual ~AggregationPhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::AGGREGATION; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override { return RC::SUCCESS; }
  Tuple *current_tuple() override;

private:
  std::vector<std::unique_ptr<Expression>> aggr_exprs_;      // 聚合表达式
  std::unique_ptr<SimpleAggregationHashTable> ht_;    // 聚合哈希表
  AggregateValue aggr_results_;
  std::unique_ptr<AggregationTuple> tuple_;
  bool is_execute_{false};
  int record_num_{0};
};
