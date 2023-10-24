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
// Created by WangYunlai on 2022/12/30.
//

#include "sql/operator/join_physical_operator.h"

NestedLoopJoinPhysicalOperator::NestedLoopJoinPhysicalOperator() {}

RC NestedLoopJoinPhysicalOperator::open(Trx *trx)
{
  if (children_.size() != 2) {
    LOG_WARN("nlj operator should have 2 children");
    return RC::INTERNAL;
  }

  RC rc = RC::SUCCESS;
  left_ = children_[0].get();
  right_ = children_[1].get();
  right_closed_ = true;
  round_done_ = true;

  rc = left_->open(trx);
  trx_ = trx;
  return rc;
}

RC NestedLoopJoinPhysicalOperator::next()
{
  bool left_need_step = (left_tuple_ == nullptr);
  RC rc = RC::SUCCESS;
  if (round_done_) {
    left_need_step = true;
  } else {
    rc = right_next();
    if (rc != RC::SUCCESS) {
      if (rc == RC::RECORD_EOF) {
        left_need_step = true;
      } else {
        return rc;
      }
    } else {
      return rc;  // got one tuple from right
    }
  }

  if (left_need_step) {
    rc = left_next();
    if (rc != RC::SUCCESS) {
      return rc;
    }
  }

  rc = right_next();
  return rc;
}

RC NestedLoopJoinPhysicalOperator::close()
{
  RC rc = left_->close();
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to close left oper. rc=%s", strrc(rc));
  }

  if (!right_closed_) {
    rc = right_->close();
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to close right oper. rc=%s", strrc(rc));
    } else {
      right_closed_ = true;
    }
  }
  return rc;
}

Tuple *NestedLoopJoinPhysicalOperator::current_tuple() { return &joined_tuple_; }

RC NestedLoopJoinPhysicalOperator::left_next()
{
  RC rc = RC::SUCCESS;
  rc = left_->next();
  if (rc != RC::SUCCESS) {
    return rc;
  }

  left_tuple_ = left_->current_tuple();
  joined_tuple_.set_left(left_tuple_);
  return rc;
}

RC NestedLoopJoinPhysicalOperator::right_next()
{
  RC rc = RC::SUCCESS;
  if (round_done_) {
    if (!right_closed_) {
      rc = right_->close();
      right_closed_ = true;
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }

    rc = right_->open(trx_);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    right_closed_ = false;

    round_done_ = false;
  }

  rc = right_->next();
  if (rc != RC::SUCCESS) {
    if (rc == RC::RECORD_EOF) {
      round_done_ = true;
    }
    return rc;
  }

  right_tuple_ = right_->current_tuple();
  joined_tuple_.set_right(right_tuple_);
  return rc;
}

HashJoinPhysicalOperator::HashJoinPhysicalOperator() {}

RC HashJoinPhysicalOperator::open(Trx* trx) {
  if (children_.size() != 2) {
    LOG_WARN("nlj operator should have 2 children");
    return RC::INTERNAL;
  }
  RC rc = RC::SUCCESS;
  left_ = children_[0].get();
  right_ = children_[1].get();

  rc = right_->open(trx);
  right_results_ = std::queue<Tuple*>();
  if (rc != RC::SUCCESS || left_->open(trx) != RC::SUCCESS) {
    return rc;
  }
  // TODO: 可能有左表达式和右表达式相反的情况, 事先做交换
  mp_.clear();
  while ((rc = right_->next()) == RC::SUCCESS) {
    auto right_tuple = right_->current_tuple();
    Value value;
    rc = right_expression()->get_value(*right_tuple, value);
    mp_[value].push_back(right_tuple);
  }
  for(auto&[k, vec]: mp_) {
    std::cout << k.to_string() << ":" << std::endl;
    for (Tuple* t: vec) {
      std::cout << t->to_string() << std::endl;
    }
  }
  trx_ = trx;
  return RC::SUCCESS;
}

RC HashJoinPhysicalOperator::next() {
  RC rc = RC::SUCCESS;
  while (right_results_.empty()) {
    rc = left_->next();
    left_tuple_ = left_->current_tuple();
    if (rc == RC::RECORD_EOF) {
      return rc;
    }
    Value value;
    rc = left_expression()->get_value(*left_tuple_, value);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    auto it = mp_.find(value);
    if (it != mp_.end()) {
      for (Tuple* right_tuple: it->second) {
        std::cout << value.to_string() << " " << right_tuple->to_string() << std::endl;
        right_results_.push(right_tuple);
      }
      break;
    }
  } 
  if (left_tuple_ == nullptr || right_results_.empty()) {
    return RC::RECORD_EOF;
  }
  joined_tuple_.set_left(left_tuple_);
  joined_tuple_.set_right(right_results_.front());
  right_results_.pop();

  return rc;
}
RC HashJoinPhysicalOperator::close() {
  RC rc = left_->close();
  if (left_->close() != RC::SUCCESS) {
    LOG_WARN("failed to close left oper. rc=%s", strrc(rc));
  }
  RC rc2 = right_->close();
  if (right_->close() != RC::SUCCESS) {
    LOG_WARN("failed to close right oper. rc=%s", strrc(rc2));
  }
  return RC::SUCCESS;
};
Tuple* HashJoinPhysicalOperator::current_tuple() {
  return &joined_tuple_;
}