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

struct SortItem
{
  std::vector<Value>* key;
  Tuple* data{nullptr};
};

public:
  OrderbyPhysicalOperator(std::vector<std::unique_ptr<FieldExpr>> &&expressions, std::vector<SortType>&& sortTypes);

  virtual ~OrderbyPhysicalOperator();

  PhysicalOperatorType type() const override { return PhysicalOperatorType::ORDERBY; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override;

private:
  std::vector<std::unique_ptr<FieldExpr>> expressions_; 
  std::vector<SortType> sort_types_; 
  std::vector<SortItem> items_;
  int idx_{0};
  Tuple* tuple_;
  
  Trx *trx_ = nullptr;
};
