/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#if defined(__clang__)
#else
#include <execution>
#endif
#include "sql/operator/orderby_physical_operator.h"

#ifdef __clang__
#define SORT(item_begin, item_end, compare_function) std::sort(item_begin, item_end, compare_function)
#else
#define SORT(item_begin, item_end, compare_function) std::sort(std::execution::par, item_begin, item_end, compare_function)
#endif


OrderbyPhysicalOperator::OrderbyPhysicalOperator(
    std::vector<std::unique_ptr<FieldExpr>> &&expressions, std::vector<SortType> &&sortTypes)
    : expressions_(std::move(expressions)), sort_types_(std::move(sortTypes))
{}

OrderbyPhysicalOperator::~OrderbyPhysicalOperator()
{
  // for (auto &item : items_) {
  //   delete item.key;
  //   delete item.data;
  // }
  for(int i = 0; i < multi_items_.size(); i++) {
    for (auto &item : multi_items_[i]) {
      delete item.key;
      delete item.data;
    }
  }
  multi_items_.clear();
}

RC OrderbyPhysicalOperator::open(Trx *trx)
{
  trx_ = trx;
  if (children_.size() != 1 || !children_[0]) {
    return RC::NOT_MATCH;
  }
  if (sort_types_.size() != expressions_.size()) {
    return RC::NOT_MATCH;
  }
  RC rc = children_[0]->open(trx);
  // items_.reserve(2048);
  int numCores = std::thread::hardware_concurrency();
  multi_items_.clear();
  multi_items_.resize(numCores);
  for (int i = 0; i < numCores; i++) {
    multi_items_[i].reserve(256);
  }

  cnt_ = 0;
  while ((rc = children_[0]->next()) == RC::SUCCESS) {
    Tuple *tuple = children_[0]->current_tuple();
    SortItem item;
    item.key = new std::vector<Value>;
    item.key->reserve(expressions_.size());
    for (int i = 0; i < expressions_.size(); i++) {
      Value value;
      rc = expressions_[i]->get_value(*tuple, value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      item.key->push_back(std::move(value));
    }
    item.data = tuple->copy_tuple();
    multi_items_[cnt_ % numCores].push_back(item);
    cnt_++;
  }

  auto sort_item = [&](int pos) {
    std::sort(multi_items_[pos].begin(), multi_items_[pos].end(), [this](const SortItem &a, const SortItem &b) -> bool{
      return this->cmp(a, b);
    });
  };
  
  // 创建多个线程，每个线程对一维进行排序
  std::vector<std::thread> threads;

  for (int i = 0; i < numCores; ++i) {
      threads.push_back(std::thread(sort_item, i));
  }
  // 等待所有线程完成
  for (auto& thread : threads) {
      thread.join();
  }
  
  for (int i = 0; i < numCores; i++) {
    if (multi_items_[i].size()) {
      heap_.push_back({i, 0});
    }
  }
  std::make_heap(heap_.begin(), heap_.end(), [&](const pii& a, const pii& b) ->bool {
    const auto& item1 = multi_items_[a.first][a.second];
    const auto& item2 = multi_items_[b.first][b.second];
    return !this->cmp(item1, item2);
  });
  return RC::SUCCESS;
}
RC OrderbyPhysicalOperator::next()
{
  if (heap_.empty()) {
    return RC::RECORD_EOF;
  }
  // tuple_ = items_[idx_].data;
  pii top = heap_[0];

  std::pop_heap(heap_.begin(), heap_.end(), [&](const pii& a, const pii& b) ->bool {
    const auto& item1 = multi_items_[a.first][a.second];
    const auto& item2 = multi_items_[b.first][b.second];
    return !this->cmp(item1, item2);
  });
  heap_.pop_back();

  if (top.second + 1 < multi_items_[top.first].size()) {
    heap_.push_back({top.first, top.second + 1});

    std::push_heap(heap_.begin(), heap_.end(), [&](const pii& a, const pii& b) ->bool {
      const auto& item1 = multi_items_[a.first][a.second];
      const auto& item2 = multi_items_[b.first][b.second];
      return !this->cmp(item1, item2);
    });
  }
  tuple_ = multi_items_[top.first][top.second].data;
  return RC::SUCCESS;
}
RC OrderbyPhysicalOperator::close()
{
  for(int i = 0; i < multi_items_.size(); i++) {
      for (auto &item : multi_items_[i]) {
        delete item.key;
        delete item.data;
      }
  }
  multi_items_.clear();
  return RC::SUCCESS;
}

Tuple *OrderbyPhysicalOperator::current_tuple() { return tuple_; }
