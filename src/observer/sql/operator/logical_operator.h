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
// Created by Wangyunlai on 2022/12/07.
//

#pragma once

#include <memory>
#include <vector>
#include <string>

#include "sql/expr/expression.h"

/**
 * @brief 逻辑算子
 * @defgroup LogicalOperator
 * @details 逻辑算子描述当前执行计划要做什么，比如从表中获取数据，过滤，投影，连接等等。
 * 物理算子会描述怎么做某件事情，这是与其不同的地方。
 */

/**
 * @brief 逻辑算子类型
 *
 */
enum class LogicalOperatorType
{
  CALC,
  TABLE_GET,    ///< 从表中获取数据
  PREDICATE,    ///< 过滤，就是谓词
  PROJECTION,   ///< 投影，就是select
  AGGREGATION,  ///< 聚合
  ORDER_BY,      ///< 聚合
  JOIN,         ///< 连接
  HASH_JOIN,    ///< hash join连表
  INSERT,       ///< 插入
  DELETE,       ///< 删除，删除可能会有子查询
  UPDATE,       ///< 更新
  EXPLAIN,      ///< 查看执行计划
};

/**
 * @brief 逻辑算子描述当前执行计划要做什么
 * @details 可以看OptimizeStage中相关的代码
 */
class LogicalOperator
{
public:
  LogicalOperator() = default;
  virtual ~LogicalOperator();

  virtual LogicalOperatorType type() const = 0;

  virtual std::string to_string() {
    switch (type()) {
      case LogicalOperatorType::TABLE_GET:
        return "TABLE_GET";
      case LogicalOperatorType::PREDICATE:
        return "PREDICATE";
      case LogicalOperatorType::PROJECTION:
        return "PROJECTION";
      case LogicalOperatorType::AGGREGATION:
        return "AGGREGATION";
      case LogicalOperatorType::ORDER_BY:
        return "ORDER_BY";
      case LogicalOperatorType::JOIN:
        return "JOIN";
      case LogicalOperatorType::HASH_JOIN:
        return "HASH_JOIN";
      case LogicalOperatorType::INSERT:
        return "INSERT";
      case LogicalOperatorType::DELETE:
        return "INSERT";
      case LogicalOperatorType::UPDATE:
        return "UPDATE";
      case LogicalOperatorType::EXPLAIN:
        return "EXPLAIN";
      default:
        break;
    }
    return "NO_SUPPORT";
  }

  virtual void printTree() {
    printTree("", this, false);
  }

  void add_child(std::unique_ptr<LogicalOperator> oper);
  std::vector<std::unique_ptr<LogicalOperator>> &children() { return children_; }
  std::vector<std::unique_ptr<Expression>> &expressions() { return expressions_; }

  void set_expressions(std::vector<std::unique_ptr<Expression>> &&expression) {
    expressions_ = std::move(expression);
  }
private:
    // 打印逻辑树
  virtual void printTree(const std::string& prefix, LogicalOperator* node, bool isFirst) {
    if (node != nullptr) {
      std::cout << prefix;
      std::cout << (isFirst ? "├──" : "└──" );
      std::cout << node->to_string() << " : ";
      for (const auto& expression: node->expressions_) {
        std::cout << expression->to_string() << " ";
      }
      std::cout << std::endl;
      for (int i = 0; i < node->children_.size(); i++) {
          printTree(prefix + (isFirst ? "│   " : "    "), node->children_[i].get(), i == 0);
      }
    }
  }

protected:
  std::vector<std::unique_ptr<LogicalOperator>> children_;  ///< 子算子

  ///< 表达式，比如select中的列，where中的谓词等等，都可以使用表达式来表示
  ///< 表达式能是一个常量，也可以是一个函数，也可以是一个列，也可以是一个子查询等等
  std::vector<std::unique_ptr<Expression>> expressions_;
};
