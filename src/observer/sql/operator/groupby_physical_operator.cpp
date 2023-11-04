
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
  // std::vector<unique_ptr<Expression>> cmp_exprs;
  // for (auto& having_unit: having_->filter_units()) {
  //   std::unique_ptr<Expression> left, right;
  //   if (having_unit->left->type() == ExprType::FIELD) {
  //     // 没调整顺序前的位置
  //     auto field_expr = static_cast<FieldExpr*>(having_unit->left.get());
  //     for (int i = 0; i < groupbys_.size(); i++) {
  //       if (groupbys_[i]->type() == ExprType::FIELD) {
  //         auto field_expr2 = static_cast<FieldExpr*>(groupbys_[i].get());
  //         if (!strcmp(field_expr->field_name(), field_expr2[i].field_name())
  //           && !strcmp(field_expr->table_name(), field_expr2[i].table_name())) {
  //             left = std::make_unique<HavingFieldExpr>(i);
  //             break;
  //         }
  //       }
  //     }
  //     if (left == nullptr) {
  //       for (int i = 0; i < aggr_exprs_.size(); i++) {
  //         if (aggr_exprs_[i]->type() == ExprType::AGGRFUNCTION) {
  //           auto field_expr2 = static_cast<AggretationExpr*>(aggr_exprs_[i].get());
  //           if (!strcmp(field_expr->field_name(), field_expr2[i].filed().field_name())
  //             && !strcmp(field_expr->table_name(), field_expr2[i].filed().table_name())) {
  //               left = std::make_unique<HavingFieldExpr>(i + groupbys_.size());
  //               break;
  //           }
  //         }
  //       }
  //     }
  //   } else if (having_unit->left->type() == ExprType::VALUE){
  //     left = std::move(having_unit->left);
  //   }

  //   if (having_unit->right->type() == ExprType::FIELD) {
  //     // 没调整顺序前的位置
  //     auto field_expr = static_cast<FieldExpr*>(having_unit->right.get());
  //     for (int i = 0; i < groupbys_.size(); i++) {
  //       if (groupbys_[i]->type() == ExprType::FIELD) {
  //         auto field_expr2 = static_cast<FieldExpr*>(groupbys_[i].get());
  //         if (!strcmp(field_expr->field_name(), field_expr2[i].field_name())
  //           && !strcmp(field_expr->table_name(), field_expr2[i].table_name())) {
  //             right = std::make_unique<HavingFieldExpr>(i);
  //             break;
  //         }
  //       }
  //     }
  //     if (right == nullptr) {
  //       for (int i = 0; i < aggr_exprs_.size(); i++) {
  //         if (aggr_exprs_[i]->type() == ExprType::FIELD) {
  //           auto field_expr2 = static_cast<AggretationExpr*>(aggr_exprs_[i].get());
  //           if (!strcmp(field_expr->field_name(), field_expr2[i].filed().field_name())
  //             && !strcmp(field_expr->table_name(), field_expr2[i].filed().table_name())) {
  //               right = std::make_unique<HavingFieldExpr>(i + groupbys_.size());
  //               break;
  //           }
  //         }
  //       }
  //     }
  //   } else if (having_unit->right->type() == ExprType::VALUE){
  //     right = std::move(having_unit->right);
  //   }

  //   if (left == nullptr || right == nullptr) {
  //     LOG_WARN("left or right: having 不支持的字段类型 ");
  //     return RC::INTERNAL;
  //   }

  //   ComparisonExpr *cmp_expr =
  //       new ComparisonExpr(having_unit->comp, std::move(left), std::move(right));
  //   cmp_exprs.emplace_back(cmp_expr);

  //   cmp_exprs.emplace_back(cmp_expr);
  // }

  // if (!cmp_exprs.empty()) {
  //   // having_expr_ = std::make_shared<ConjunctionExpr>(having_->is_or ? ConjunctionExpr::Type::OR :
  //   ConjunctionExpr::Type::AND, cmp_exprs); having_expr_ = new ConjunctionExpr(having_->is_or ?
  //   ConjunctionExpr::Type::OR : ConjunctionExpr::Type::AND, cmp_exprs);
  // }

  return RC::SUCCESS;
}

RC GroupbyPhysicalOperator::open(Trx *trx)
{
  return RC::SUCCESS;
  // if (children_.empty()) {
  //   return RC::INTERNAL;
  // }
  // const auto child_op = children_.front().get();
  // RC rc = child_op->open(trx);
  // Tuple *tuple;
  // ht_.clear();
  // while ((rc = child_op->next()) == RC::SUCCESS) {
  //   tuple = child_op->current_tuple();

  //   AggregateKey key;         // 获得grouby聚合的key
  //   key.group_bys_.reserve(groupbys_.size());
  //   for (int i = 0; i < groupbys_.size(); i++) {
  //     Value value;
  //     rc = groupbys_[i]->get_value(*tuple, value);
  //     if (rc != RC::SUCCESS) {
  //       return rc;
  //     }
  //     key.group_bys_.emplace_back(value);
  //   }

  //   // 再获得需要聚合的value
  //   AggregateValue values;
  //   values.aggregates_.reserve(aggr_exprs_.size());
  //   for (int i = 0; i < aggr_exprs_.size(); i++) {
  //     Value value;
  //     rc = aggr_exprs_[i]->get_value(*tuple, value);
  //     if (rc != RC::SUCCESS) {
  //       return rc;
  //     }
  //     values.aggregates_.emplace_back(value);
  //   }

  //   auto it = ht_.find(key);
  //   if (it == ht_.end()) {
  //     ht_.emplace(key, SimpleAggregationHashTable(&aggr_exprs_));
  //     it = ht_.find(key);
  //     it->second.init();
  //   }
  //   it->second.combine_aggregate_values(values);
  // }
  // if (rc == RC::RECORD_EOF) {
  //   rc = RC::SUCCESS;
  // }
  // it_ = ht_.begin();
  // std::vector<Value> empty_values(query_fields_.size());
  // tuple_ = std::make_unique<AggregationTuple>(empty_values, query_fields_);

  // // 1.处理输出顺序的问题
  // // 找query_fields_每一项，在最终聚合tuple中的位置
  // // relation[i] 表示，query_fields_[i] 在最终返回tuple中的下标
  // relations_.clear();
  // for (int i = 0, k = 0; i < query_fields_.size(); i++) {
  //   if (!query_fields_[i].with_aggr()) {
  //     for (int j = 0; j < groupbys_.size(); j++) {
  //       auto field_expr = static_cast<FieldExpr*>(groupbys_[j].get());
  //       if (!strcmp(field_expr->field_name(), query_fields_[i].field_name())
  //         && !strcmp(field_expr->table_name(), query_fields_[i].table_name())) {
  //           relations_.push_back(j);
  //           break;
  //       }
  //     }
  //   } else {
  //     // 找出聚合字段
  //     relations_.push_back(groupbys_.size() + k);
  //     k++;
  //   }
  // }
  // // 映射大小必须相等
  // if (relations_.size() != query_fields_.size()) {
  //   return RC::INTERNAL;
  // }
  // 2.处理having的过滤, having的表达式可以是值，也可以是query_fields_中的一个
  // rc = create_having_expression();
  // return rc;
}

RC GroupbyPhysicalOperator::predicate(const Tuple &tuple, bool &res)
{
  // res = true;
  // if (having_expr_ != nullptr) {
  //   Value value;
  //   RC rc = having_expr_->get_value(tuple, value);
  //   res = value.get_boolean();
  //   return rc;
  // }
  return RC::SUCCESS;
}

RC GroupbyPhysicalOperator::next()
{
  return RC::RECORD_EOF;
  // if (it_ == ht_.end()) {
  //   return RC::RECORD_EOF;
  // }
  // bool is_find = false;
  // RC rc = RC::SUCCESS;
  // while (it_ != ht_.end()) {
  //   std::vector<Value> values;
  //   // 这里会根据having，和query_fields对结果进行一次 重新排序
  //   it_->second.generate_aggregate_values();
  //   values.insert(values.end(), it_->first.group_bys_.begin(), it_->first.group_bys_.end());
  //   values.insert(values.end(), it_->second.aggr_results().aggregates_.begin(),
  //   it_->second.aggr_results().aggregates_.end()); tuple_->set_aggregations(values); bool res; rc =
  //   predicate(*tuple_, res); if (rc != RC::SUCCESS) {
  //     return rc;
  //   }
  //   if (res) {
  //     it_++;
  //     is_find = true;
  //     std::vector<Value> new_values;
  //     for (int x: relations_) {
  //       new_values.emplace_back(values[x]);
  //     }
  //     tuple_->set_aggregations(new_values);
  //     break;
  //   }
  //   it_++;
  // }
  // if (!is_find) {
  //   rc = RC::RECORD_EOF;
  // }
  // return rc;
}

Tuple *GroupbyPhysicalOperator::current_tuple() { return tuple_.get(); }

RC GroupbyPhysicalOperator::close()
{
  // ht_.clear();
  return RC::SUCCESS;
}
