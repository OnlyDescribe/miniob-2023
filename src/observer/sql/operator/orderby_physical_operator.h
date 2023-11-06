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

#include "sql/operator/physical_operator.h"

class Trx;

/**
 * @brief 物理算子，删除
 * @ingroup PhysicalOperator
 */
class OrderbyPhysicalOperator : public PhysicalOperator
{

using pii = pair<int,int>;

  struct SortItem
  {
    std::vector<Value> *key;
    Tuple *data{nullptr};
  };

public:
  OrderbyPhysicalOperator(std::vector<std::unique_ptr<FieldExpr>> &&expressions, std::vector<SortType> &&sortTypes);

  virtual ~OrderbyPhysicalOperator();

  PhysicalOperatorType type() const override { return PhysicalOperatorType::ORDERBY; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override;

  bool cmp(const SortItem &a, const SortItem &b) {
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
  };

  bool heap_cmp(const pii& a, const pii& b) {
    const auto& item1 = multi_items_[a.first][a.second];
    const auto& item2 = multi_items_[b.first][b.second];
    return !cmp(item1, item2);
  }
private:
  std::vector<std::unique_ptr<FieldExpr>> expressions_;
  std::vector<SortType> sort_types_;

  std::vector<vector<SortItem>> multi_items_;
  std::vector<std::pair<int,int>> heap_;;
  std::vector<SortItem> items_;
  int idx_{0};
  int cnt_{0};
  Tuple *tuple_;

  Trx *trx_ = nullptr;
};
