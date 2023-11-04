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

/** AggregateKey represents a key in an aggregation operation */
struct GroupbyKey
{
  /** The group-by values */
  std::vector<Value> group_bys_;

  /**
   * Compares two aggregate keys for equality.
   * @param other the other aggregate key to be compared with
   * @return `true` if both aggregate keys have equivalent group-by expressions, `false` otherwise
   */
  auto operator==(const GroupbyKey &other) const -> bool
  {
    for (uint32_t i = 0; i < other.group_bys_.size(); i++) {
      if (other.group_bys_[i].is_null() && group_bys_[i].is_null()) {
        return true;
      } else if (other.group_bys_[i].is_null() || group_bys_[i].is_null()) {
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
struct hash<GroupbyKey>
{
  auto operator()(const GroupbyKey &agg_key) const -> std::size_t
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

}  

/**
 * @brief 聚合算子, 下边一般挂一个全表扫描的算子
 * @ingroup PhysicalOperator
 */
class GroupbyPhysicalOperator : public PhysicalOperator
{

public:
  GroupbyPhysicalOperator(std::vector<std::unique_ptr<Expression>> &&projcets, const FilterStmt *having,
      std::vector<std::unique_ptr<Expression>> &&groupbys)
      : projcets_(std::move(projcets)), having_(having), groupbys_(std::move(groupbys))
  {}

  // TODO: having_expr_会内存泄漏，但是如果加上delete后，这里有一个core
  virtual ~GroupbyPhysicalOperator()
  {
    if (having_expr_ != nullptr) {
      delete having_expr_;
    }
  }

  PhysicalOperatorType type() const override { return PhysicalOperatorType::GROUPBY; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;
  Tuple *current_tuple() override;

  virtual TupleSchema tuple_schema() const override
  {
    TupleSchema project_schema;
    for (const auto& expr: projcets_) {
      project_schema.append_cell(expr->name().c_str());
    }
    return project_schema;
  }

private:
  RC predicate(const Tuple &tuple, bool &res);
  RC create_having_expression();  // 将having语句处理成conjunction expression

private:
  std::vector<std::unique_ptr<Expression>> projcets_;                // 聚合表达式
  std::vector<Expression*> ex_exprs_;                                 // 除了投影，带上having聚合的表达式
  std::vector<std::unique_ptr<Expression>> groupbys_;                // groupby 子句, 应该是FieldExpr
  const FilterStmt *having_{nullptr};                                 // having, not own this;

  SimpleAggregationHashTable<GroupbyKey> ht_;
  SimpleAggregationHashTable<GroupbyKey>::Iterator iter_;
  ValueListTuple tuple_;

  // std::shared_ptr<ConjunctionExpr> having_expr_;
  ConjunctionExpr *having_expr_{nullptr};  // 用于having的过滤
};
