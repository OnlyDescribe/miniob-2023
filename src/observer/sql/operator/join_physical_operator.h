/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by WangYunlai on 2021/6/10.
//

#pragma once

#include <memory>

#include "sql/parser/parse.h"
#include "sql/operator/physical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/aggregation_physical_operator.h"

/**
 * @brief 最简单的两表（称为左表、右表）join算子
 * @details 依次遍历左表的每一行，然后关联右表的每一行
 * @ingroup PhysicalOperator
 */
class NestedLoopJoinPhysicalOperator : public PhysicalOperator
{
public:
  NestedLoopJoinPhysicalOperator();
  virtual ~NestedLoopJoinPhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::NESTED_LOOP_JOIN; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;
  Tuple *current_tuple() override;

  void set_expressions(std::vector<std::unique_ptr<Expression>> &&expressions)
  {
    expressions_ = std::move(expressions);
  }

  RC predicate();

private:
  RC left_next();   //! 左表遍历下一条数据
  RC right_next();  //! 右表遍历下一条数据，如果上一轮结束了就重新开始新的一轮

private:
  Trx *trx_ = nullptr;

  //! 左表右表的真实对象是在PhysicalOperator::children_中，这里是为了写的时候更简单
  PhysicalOperator *left_ = nullptr;
  PhysicalOperator *right_ = nullptr;
  Tuple *left_tuple_ = nullptr;
  Tuple *right_tuple_ = nullptr;
  JoinedTuple joined_tuple_;                              //! 当前关联的左右两个tuple
  bool round_done_ = true;                                //! 右表遍历的一轮是否结束
  bool right_closed_ = true;                              //! 右表算子是否已经关闭
  std::vector<std::unique_ptr<Expression>> expressions_;  //! 值表达式
};

/**
 * @description: hash join
 */
// TODO: 更新初始化函数, 这里只涉及跨表比较函数，剩下的已经下推
class HashJoinPhysicalOperator : public PhysicalOperator
{
public:
  HashJoinPhysicalOperator();
  virtual ~HashJoinPhysicalOperator()
  {
    for (auto &[k, tuple_vec] : mp_) {
      for (auto tuple : tuple_vec) {
        delete tuple;
      }
    }
  }

  PhysicalOperatorType type() const override { return PhysicalOperatorType::HASH_JOIN; }
  void set_expressions(std::vector<std::unique_ptr<Expression>> &&expressions)
  {
    expressions_ = std::move(expressions);
  }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;
  RC predicate();
  Tuple *current_tuple() override;

private:
  void init_expressions();
  Trx *trx_ = nullptr;

  //! 左表右表的真实对象是在PhysicalOperator::children_中，这里是为了写的时候更简单
  PhysicalOperator *left_ = nullptr;
  PhysicalOperator *right_ = nullptr;
  Tuple *left_tuple_ = nullptr;
  JoinedTuple joined_tuple_;                                    //! 当前关联的左右两个tuple
  std::unordered_map<AggregateKey, std::vector<Tuple *>> mp_;   // 存放右表的数据结构
  std::vector<std::unique_ptr<Expression>> expressions_;        // 存放比较表达式，用于过滤
  std::vector<std::unique_ptr<Expression>> left_expressions_;   // 用于左表的hash join
  std::vector<std::unique_ptr<Expression>> right_expressions_;  // 用于右表的hash join

  std::queue<Tuple *> right_results_;                           // 和当前left_tuple匹配的右表
};
