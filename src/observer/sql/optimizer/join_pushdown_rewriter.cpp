/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */


#include "sql/optimizer/join_pushdown_rewriter.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/expr/expression.h"

RC JoinPushdownRewriter::rewrite(std::unique_ptr<LogicalOperator> &oper, bool &change_made)
{
  RC rc = RC::SUCCESS;
  // 当前不是join，先递归下面的算子
  if (oper->type() != LogicalOperatorType::JOIN) {
    return rc;
  }
  auto join_op = static_cast<JoinLogicalOperator*>(oper.get());
  if (oper->children().size() != 2) {
    return rc;
  }
  std::unique_ptr<LogicalOperator>& left_child = oper->children()[0];
  std::unique_ptr<LogicalOperator>& right_child = oper->children()[1];

  if (right_child->type() != LogicalOperatorType::TABLE_GET) {
    return rc;
  } 

  auto table_get_oper = static_cast<TableGetLogicalOperator *>(right_child.get());
  std::vector<std::unique_ptr<Expression>> pushdown_left_exprs;
  std::vector<std::unique_ptr<Expression>> pushdown_right_exprs;
  // 当前的表达式
  std::vector<std::unique_ptr<Expression>> &join_oper_exprs = oper->expressions();
  rc = get_exprs_can_pushdown(join_op, join_oper_exprs, pushdown_left_exprs, pushdown_right_exprs, change_made);

  // 下推函数
  auto push_down = [&change_made](LogicalOperator* child, std::vector<std::unique_ptr<Expression>>& pushdown_exprs) {
    while (!pushdown_exprs.empty()) {
      if (child->type() == LogicalOperatorType::TABLE_GET) {
        auto table_scan = static_cast<TableGetLogicalOperator*>(child);
        table_scan->predicates().emplace_back(std::move(pushdown_exprs.back()));
      } else {
        child->add_expressioin(std::move(pushdown_exprs.back()));
      }
      pushdown_exprs.pop_back();
      change_made |= true;
    }
  };
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get exprs can pushdown. rc=%s", strrc(rc));
    return rc;
  }
  // 执行下推
  push_down(left_child.get(), pushdown_left_exprs);
  push_down(right_child.get(), pushdown_right_exprs);

  for (std::unique_ptr<LogicalOperator> &child_oper : oper->children()) {
    bool sub_change_made = false;
    rc = rewrite(child_oper, sub_change_made);
    if (sub_change_made && !change_made) {
      change_made = true;
    }
    if (rc != RC::SUCCESS) {
      break;
    }
  }
  return rc;
}


/**
 * 查看表达式是否可以直接下放到table get算子的filter
 * @param expr 是当前的表达式。如果可以下放给table get 算子，执行完成后expr就失效了
 * @param pushdown_exprs 当前所有要下放给table get 算子的filter。此函数执行多次，
 *                       pushdown_exprs 只会增加，不要做清理操作
 */
RC JoinPushdownRewriter::get_exprs_can_pushdown(JoinLogicalOperator* join_oper,
      std::vector<std::unique_ptr<Expression>> &exprs, 
      std::vector<std::unique_ptr<Expression>> &pushdown_left_exprs,
      std::vector<std::unique_ptr<Expression>> &pushdown_right_exprs, bool& change_made)
{
  RC rc = RC::SUCCESS;
  if (join_oper == nullptr || join_oper->right_table == nullptr) {
    return rc;
  }
  auto right_table = join_oper->right_table;
  // TODO: 默认join的表达式都是用 and 连接的，OR的情况 以后再考虑？
  for (auto it = exprs.begin(); it != exprs.end(); ) {
    if ((*it)->type() != ExprType::COMPARISON) {
      // 值表达式
      if ((*it)->type() == ExprType::VALUE) {
        auto value_expr = static_cast<ValueExpr*>((*it).get());
        Value value;
        value_expr->get_value(value);
        if (value.attr_type() == AttrType::BOOLEANS) {
          if (value.get_boolean()) {
            change_made |= true;
            it = exprs.erase(it);
          } else {
            change_made |= true;
            join_oper->children().clear();
            return rc;
          }
        }
      } else {
        it++;
      }
      continue;
    }
    auto comparison_expr = static_cast<ComparisonExpr *>((*it).get());
    auto& left = comparison_expr->left();
    auto& right = comparison_expr->right();
    if (left->type() != ExprType::FIELD && left->type() != ExprType::VALUE) {
      it++;
      continue;
    }
    if (right->type() != ExprType::FIELD && right->type() != ExprType::VALUE) {
      it++;
      continue;
    }
    // t1.a compare t2.b 的表达式
    // 1. 两个都是属性表达式
    //    1.1 如果两个都在左边，或者两个都在右边，则执行算子下推
    //    1.2 如果分开
    //       1.2.1 如果是=, 那么可以使用hash join
    //       1.2.2 如果是a.id > b.id, 那么必须使用nest_loop_join
    // 2. 两个值表达式
    //    3.1 如果不成立，这次join就是空, 就是一个空表
    //    3.2 如果成立，那么放弃这个unit
    // 3. 一个属性 一个值表达式
    //    算子执行下推到对应的左右子树, 这里使用predict来实现过滤
    if (left->type() == ExprType::FIELD && right->type() == ExprType::FIELD) {
      const auto left_expr = static_cast<FieldExpr*>(left.get());
      const auto right_expr = static_cast<FieldExpr*>(right.get());
      bool left_unit_in_left = (strcmp(left_expr->table_name(), right_table->name()) != 0);
      bool right_unit_in_left = (strcmp(right_expr->table_name(), right_table->name()) != 0);
      if (left_unit_in_left && right_unit_in_left) { // 都在左边
          pushdown_left_exprs.emplace_back(std::move(*it));
          it = exprs.erase(it);
        } else if (!left_unit_in_left && !right_unit_in_left) { // 都在右边
          pushdown_right_exprs.emplace_back(std::move(*it));
          it = exprs.erase(it);
        } else {
          // 一左一右, 把顺序换对，不需要下推（留着), 但是比较符号需要转化
          if (!left_unit_in_left) { 
            left.swap(right);
            comparison_expr->reverse_comp();
          }
          it++;
      }
    } else if (left->type() == ExprType::VALUE && right->type() == ExprType::VALUE) {
      Value value;
      rc = comparison_expr->try_get_value(value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      if (value.get_boolean()) { // 可以删除这个表达式
          change_made |= true;
          it = exprs.erase(it);
      } else {        // 因为是 AND 连接，如果不相等，可以直接删除这颗子树 (把孩子都删除)
        join_oper->children().clear();
        change_made |= true;
        return rc;
      }
    } else if(left->type() == ExprType::FIELD && right->type() == ExprType::VALUE) {
      const auto left_expr = static_cast<FieldExpr*>(left.get());
      const auto right_expr = static_cast<ValueExpr*>(right.get());
      if (!strcmp(left_expr->table_name(), right_table->name())) {
        pushdown_right_exprs.emplace_back(std::move(*it));
      } else {
        pushdown_left_exprs.emplace_back(std::move(*it));
      }
      it = exprs.erase(it);
    } else {
      const auto left_expr = static_cast<ValueExpr*>(left.get());
      const auto right_expr = static_cast<FieldExpr*>(right.get());
      if (!strcmp(right_expr->table_name(), right_table->name())) {
        pushdown_right_exprs.emplace_back(std::move(*it));
      } else {
        pushdown_left_exprs.emplace_back(std::move(*it));
      }
      it = exprs.erase(it);  
    } 
  }
  return rc;
}

