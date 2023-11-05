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
        case AttrType::BOOLEANS: valueHash = std::hash<bool>{}(value.get_boolean()); break;
        case AttrType::CHARS: valueHash = std::hash<std::string>{}(value.get_string()); break;
        case AttrType::NULLS: valueHash = std::hash<int>{}(-182301); break;  // null就随缘大法
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

template <typename T>
class SimpleAggregationHashTable
{

public:
  /**
   * Construct a new SimpleAggregationHashTable instance.
   * @param agg_exprs the aggregation expressions
   * @param agg_types the types of aggregations
   */
  SimpleAggregationHashTable() {}

  void init(const std::vector<Expression *> &agg_exprs)
  {
    agg_exprs_.clear();
    for (auto &agg_expr : agg_exprs) {
      agg_exprs_.push_back(agg_expr);
    }
  }

  void init(const std::vector<std::unique_ptr<Expression>> &agg_exprs)
  {
    agg_exprs_.clear();
    for (auto &agg_expr : agg_exprs) {
      agg_exprs_.push_back(agg_expr.get());
    }
  }
  /**
   * @description: 因为平均数只是和，需要计算最终的结果
   * @return {*}
   */
  void generate_aggregate_values()
  {
    // 更改avg的结果, 统一改成float
    for (auto it = ht_.begin(); it != ht_.end(); it++) {
      auto not_null_it = null_not_record_.find(it->first);
      for (int i = 0; i < it->second.aggregates_.size(); i++) {
        if (agg_exprs_[i]->type() == ExprType::AGGRFUNCTION &&
            !not_null_it->second.aggregates_[i].is_null() &&
            static_cast<AggretationExpr *>(agg_exprs_[i])->aggr_type() == AggrFuncType::AVG) {
          it->second.aggregates_[i] =
              Value(it->second.aggregates_[i].get_float() / not_null_it->second.aggregates_[i].get_int());
        }
      }
    }
  }

  void clear() { ht_.clear(); }

  AggregateValue get_aggr_value(const T &agg_key)
  {
    auto it = ht_.find(agg_key);
    if (it == ht_.end()) {
      return generate_initial_aggregateValue();
    }
    return it->second;
  }

  void insert_combine(const T &agg_key, const Tuple &tuple)
  {
    if (ht_.count(agg_key) == 0) {
      ht_.insert({agg_key, generate_initial_aggregateValue()});
      null_not_record_.insert({agg_key, generate_initial_aggregateValue()});
    }
    combine_aggregate_values(&ht_[agg_key], &null_not_record_[agg_key], tuple);
  }
    /** An iterator over the aggregation hash table */
  class Iterator {
    public:
        Iterator() {};
        Iterator(typename std::unordered_map<T, AggregateValue>::const_iterator iter)
            : iter_(iter) {}

        const T& Key() const {
            return iter_->first;
        }

        const AggregateValue& Val() {
            return iter_->second;
        }

        bool operator==(const Iterator& other) const {
            return iter_ == other.iter_;
        }

        bool operator!=(const Iterator& other) const {
            return iter_ != other.iter_;
        }

        Iterator& operator++() {
            ++iter_;
            return *this;
        }

    private:
        typename std::unordered_map<T, AggregateValue>::const_iterator iter_;
    };

    Iterator begin() const {
        return Iterator(ht_.begin());
    }

    Iterator end() const {
        return Iterator(ht_.end());
    }
private:
  // 将b聚合到a, not_null表示记录a非空的个数(需要计算平均值)
  void aggregate_value(const Value &b, Value &a, Value &not_null, AggrFuncType type)
  {
    // 如果需要，先初始化
    auto init_value = [](const Value &intial_value, Value &output) -> bool {
      if (output.attr_type() == AttrType::NULLS) {
        output = intial_value;
        return true;
      }
      return false;
    };
    Value res;
    switch (type) {
      case AggrFuncType::COUNT_STAR: a = Value::add(a, Value(1)); break;
      case AggrFuncType::COUNT:
        if (!b.is_null()) {
          a = Value::add(a, Value(1));
        }
        break;
      case AggrFuncType::AVG:
        if (!b.is_null()) {
          if (a.is_null()) {
            not_null = Value(1);
            a = b;
          } else {
            a = Value::add(a, b);
            not_null = Value::add(not_null, Value(1));
          }
        }
        break;
      case AggrFuncType::SUM:
        if (!b.is_null() && !init_value(b, a)) {
          a = Value::add(a, b);
        }
        break;
      case AggrFuncType::MIN:
        if (!b.is_null() && !init_value(b, a)) {
          if (b.compare(a) < 0) {
            a = b;
          }
        }
        break;
      case AggrFuncType::MAX:
        if (!b.is_null() && !init_value(b, a)) {
          if (b.compare(a) > 0) {
            a = b;
          }
        }
        break;
      default: LOG_ERROR("aggretion invalid"); break;
    }
  }
  /** @return The initial aggregate value for this aggregation executor */
  auto generate_initial_aggregateValue() -> AggregateValue
  {
    std::vector<Value> values{};
    for (const auto &agg_expr_ptr : agg_exprs_) {
      if (agg_expr_ptr->type() == ExprType::AGGRFUNCTION) {
        auto aggr_expr = static_cast<AggretationExpr *>(agg_expr_ptr);
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
      } else if (agg_expr_ptr->type() == ExprType::FIELD) {
        values.emplace_back(Value(AttrType::NULLS));
      } else {
        LOG_WARN("agg_expr_ not implement");
      }
    }
    return {values};
  }

  /**
   *
   * Combines the input into the aggregation result.
   * @param input The input value
   */
  void combine_aggregate_values(AggregateValue *results, AggregateValue *null_not_record, const Tuple &tuple)
  {
    for (uint32_t i = 0; i < agg_exprs_.size(); i++) {
      if (agg_exprs_[i]->type() == ExprType::AGGRFUNCTION) {
        auto aggr_expr = static_cast<AggretationExpr *>(agg_exprs_[i]);
        Value value;
        aggr_expr->get_value(tuple, value);
        aggregate_value(value, results->aggregates_[i], null_not_record->aggregates_[i], aggr_expr->aggr_type());
      } else {
        assert(agg_exprs_[i]->type() == ExprType::FIELD);
        // auto field_expr = static_cast<FieldExpr *>(agg_exprs_[i]);
        RC rc = agg_exprs_[i]->get_value(tuple, results->aggregates_[i]);
        if (rc != RC::SUCCESS) {
          LOG_WARN("combine not success");
        }
      }
    }
  }
  /** The hash table is just a map from aggregate keys to aggregate values */
  std::unordered_map<T, AggregateValue> ht_{};
  // std::unordered_map<T, AggregateValue>::iterator;
  std::unordered_map<T, AggregateValue> null_not_record_{};
  std::vector<Expression *> agg_exprs_{nullptr};  // not own this
  // AggregateValue results_;
};

/**
 * @brief 聚合算子, 下边一般挂一个全表扫描的算子
 * @ingroup PhysicalOperator
 */
class AggregationPhysicalOperator : public PhysicalOperator
{

public:
  AggregationPhysicalOperator(std::vector<std::unique_ptr<Expression>> &&aggr_exprs) : projects_(std::move(aggr_exprs))
  {
    ht_ = std::make_unique<SimpleAggregationHashTable<AggregateKey>>();
  }

  virtual ~AggregationPhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::AGGREGATION; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override { return RC::SUCCESS; }
  Tuple *current_tuple() override;

  virtual TupleSchema tuple_schema() const override
  {
    TupleSchema schema;
    for (const auto &expr : projects_) {
      schema.append_cell(expr->name().c_str());
    }
    return schema;
  }

private:
  std::vector<std::unique_ptr<Expression>> projects_;  // 投影
  std::vector<Expression *> aggr_exprs_;               // 聚合表达式
  std::unique_ptr<SimpleAggregationHashTable<AggregateKey>> ht_;     // 聚合哈希表
  std::unique_ptr<AggregationTuple> aggregation_tuple_;

  ExpressionTuple exprssion_tuple_;  // 表达式
  bool is_execute_{false};
};
