
/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/operator/groupby_physical_operator.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/table/table.h"
#include "event/sql_debug.h"

using namespace std;

RC GroupbyPhysicalOperator::create_having_expression()
{
  // 先放入投影
  for (const auto& project: projcets_) {
    ex_exprs_.push_back(project.get());
  }
  std::vector<unique_ptr<Expression>> cmp_exprs;
  // 再把having部分放进去, 顺便构造过滤表达式
  for (auto& having_unit: having_->filter_units()) {
    std::unique_ptr<Expression> left, right;
    // 只考虑聚合的情况
    if (having_unit->left->type() == ExprType::AGGRFUNCTION) {
      // 聚合后的位置
      left = std::make_unique<HavingFieldExpr>(ex_exprs_.size());
      ex_exprs_.push_back(having_unit->left.get());
    } else if (having_unit->left->type() == ExprType::VALUE) {  
      left = std::move(having_unit->left);
    } else {
      LOG_WARN("having only support VALUE and AGGRFUNCTION");
      return RC::NOT_IMPLEMENT;
    }
    // 只考虑聚合的长度
    if (having_unit->right->type() == ExprType::AGGRFUNCTION) {
      // 聚合后的位置
      right = std::make_unique<HavingFieldExpr>(ex_exprs_.size());
      ex_exprs_.push_back(having_unit->right.get());
    } else if (having_unit->right->type() == ExprType::VALUE) {  
      right = std::move(having_unit->right);
    } else {
      LOG_WARN("having only support VALUE and AGGRFUNCTION");
      return RC::NOT_IMPLEMENT;
    }

    ComparisonExpr *cmp_expr =
        new ComparisonExpr(having_unit->comp, std::move(left), std::move(right));
    cmp_exprs.emplace_back(cmp_expr);
  }

  if (!cmp_exprs.empty()) {
    // having_expr_ = std::make_shared<ConjunctionExpr>(having_->is_or ? ConjunctionExpr::Type::OR :
    // ConjunctionExpr::Type::AND, cmp_exprs); 
    having_expr_ = new ConjunctionExpr(having_->is_or ?
    ConjunctionExpr::Type::OR : ConjunctionExpr::Type::AND, cmp_exprs);
  }

  return RC::SUCCESS;
}

RC GroupbyPhysicalOperator::open(Trx *trx)
{
  // 2.处理having的过滤, having的表达式可以是值，也可以是query_fields_中的一个
  if (children_.empty()) {
    return RC::INTERNAL;
  }
  const auto child_op = children_.front().get();
  RC rc = child_op->open(trx);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  rc = create_having_expression();
  Tuple *tuple;
  ht_.clear();
  ht_.init(ex_exprs_);

  while ((rc = child_op->next()) == RC::SUCCESS) {
    tuple = child_op->current_tuple();
    GroupbyKey key;         // 获得grouby聚合的key
    key.group_bys_.reserve(groupbys_.size());
    for (int i = 0; i < groupbys_.size(); i++) {
      Value value;
      rc = groupbys_[i]->get_value(*tuple, value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      key.group_bys_.emplace_back(value);
    }
    ht_.insert_combine(key, *tuple);
  }

  if (rc == RC::RECORD_EOF) {
    rc = RC::SUCCESS;
  }

  iter_ = ht_.begin();
  ht_.generate_aggregate_values();
  return rc;
}

RC GroupbyPhysicalOperator::predicate(const Tuple &tuple, bool &res)
{
  res = true;
  if (having_expr_ != nullptr) {
    Value value;
    RC rc = having_expr_->get_value(tuple, value);
    res = value.get_boolean();
    return rc;
  }
  
  return RC::SUCCESS;
}

RC GroupbyPhysicalOperator::next()
{
  if (iter_ == ht_.end()) {
    return RC::RECORD_EOF;
  }
  bool is_find = false;
  while (iter_ != ht_.end()) {
    tuple_.set_cells(iter_.Val().aggregates_);
    bool res;
    RC rc = predicate(tuple_, res);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    if (res) {
      is_find = true;
      ++iter_;
      break;
    } else {
      ++iter_;
    }
  }
  return is_find ? RC::SUCCESS : RC::RECORD_EOF;
}

Tuple *GroupbyPhysicalOperator::current_tuple() { return &tuple_; }

RC GroupbyPhysicalOperator::close()
{
  ht_.clear();
  return RC::SUCCESS;
}
