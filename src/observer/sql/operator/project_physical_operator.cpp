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
// Created by WangYunlai on 2022/07/01.
//

#include "common/log/log.h"
#include "sql/operator/project_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"



ProjectPhysicalOperator::ProjectPhysicalOperator(std::vector<std::unique_ptr<Expression>>&& exprs): 
  exprs_(std::move(exprs)) 
{
  // for (const auto& expr: exprs_) {
  //   if (expr->type() == ExprType::FIELD) {
  //     auto field_expr = static_cast<FieldExpr*>(expr.get());
  //     auto table = field_expr->field().table();
  //     auto field_meta = field_expr->field().meta();
  //     TupleCellSpec *spec = new TupleCellSpec(table->name(), field_meta->name(), field_meta->name());
  //     tuple_.add_cell_spec(spec);
  //   } else if (expr->type() == ExprType::ARITHMETIC) {
  //     // tuple_.ad
  //   }
  // }
  tuple_.set_expressions(&exprs_);
}

RC ProjectPhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  PhysicalOperator *child = children_[0].get();
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  return RC::SUCCESS;
}

RC ProjectPhysicalOperator::next()
{
  if (children_.empty()) {
    return RC::RECORD_EOF;
  }
  return children_[0]->next();
}

RC ProjectPhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}
Tuple *ProjectPhysicalOperator::current_tuple()
{
  tuple_.set_tuple(children_[0]->current_tuple());
  return &tuple_;
}

