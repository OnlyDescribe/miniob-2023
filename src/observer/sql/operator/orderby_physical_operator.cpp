/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#include "sql/operator/orderby_physical_operator.h"

OrderbyPhysicalOperator::OrderbyPhysicalOperator(
    std::vector<std::unique_ptr<FieldExpr>> &&expressions, std::vector<SortType> &&sortTypes)
    : expressions_(std::move(expressions)), sort_types_(std::move(sortTypes))
{}

OrderbyPhysicalOperator::~OrderbyPhysicalOperator()
{
  for (auto &item : items_) {
    delete item.key;
    delete item.data;
  }
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
  while ((rc = children_[0]->next()) == RC::SUCCESS) {
    Tuple *tuple = children_[0]->current_tuple();
    SortItem item;
    item.key = new std::vector<Value>;
    for (int i = 0; i < expressions_.size(); i++) {
      Value value;
      rc = expressions_[i]->get_value(*tuple, value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      item.key->emplace_back(value);
    }
    item.data = tuple->copy_tuple();
    items_.push_back(item);
  }
  // vector<Tuple*>
  // 内存排序模型
  std::sort(items_.begin(), items_.end(), [&](const SortItem &a, const SortItem &b) -> bool {
    assert(a.key->size() == b.key->size());
    assert(a.key->size() == sort_types_.size());
    for (int i = 0; i < a.key->size(); i++) {
      if ((*a.key)[i].attr_type() == AttrType::NULLS && (*b.key)[i].attr_type() != AttrType::NULLS) {
        return sort_types_[i] == SortType::ASC ? true : false;
      } else if ((*a.key)[i].attr_type() != AttrType::NULLS && (*b.key)[i].attr_type() == AttrType::NULLS) {
        return sort_types_[i] == SortType::ASC ? false : true;
      } else if ((*a.key)[i].attr_type() == AttrType::NULLS && (*b.key)[i].attr_type() == AttrType::NULLS) {
        continue;
      } else {
        int ret = (*a.key)[i].compare((*b.key)[i]);
        // -1, a<b, 0: ret=0, 1,
        if (ret != 0) {
          bool result = ret < 0;
          // 默认升序
          if (sort_types_[i] == SortType::DESC) {
            result = !result;
          }
          return result;
        }
      }
    }
    return false;
  });
  idx_ = -1;
  return RC::SUCCESS;
}
RC OrderbyPhysicalOperator::next()
{
  if (++idx_ >= items_.size()) {
    return RC::RECORD_EOF;
  }
  tuple_ = items_[idx_].data;
  return RC::SUCCESS;
}
RC OrderbyPhysicalOperator::close()
{
  for (auto &item : items_) {
    delete item.key;
    delete item.data;
  }
  items_.clear();
  return RC::SUCCESS;
}

Tuple *OrderbyPhysicalOperator::current_tuple() { return tuple_; }
