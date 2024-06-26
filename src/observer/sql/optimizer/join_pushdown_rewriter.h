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

#include <vector>
#include "sql/optimizer/rewrite_rule.h"

class JoinLogicalOperator;
/**
 * @brief 将join的条件下推到左右到子节点中
 * @ingroup Rewriter
 * @details 这样可以提前过滤一些数据
 */
class JoinPushdownRewriter : public RewriteRule
{
public:
  JoinPushdownRewriter() = default;
  virtual ~JoinPushdownRewriter() = default;

  RC rewrite(std::unique_ptr<LogicalOperator> &oper, bool &change_made) override;

private:
  RC get_exprs_can_pushdown(JoinLogicalOperator *join_oper, std::vector<std::unique_ptr<Expression>> &exprs,
      std::vector<std::unique_ptr<Expression>> &pushdown_left_exprs,
      std::vector<std::unique_ptr<Expression>> &pushdown_right_exprs, bool &change_made);
};
