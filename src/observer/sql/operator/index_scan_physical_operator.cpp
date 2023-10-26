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
// Created by Wangyunlai on 2022/07/08.
//

#include "sql/operator/index_scan_physical_operator.h"
#include "common/lang/bitmap.h"
#include "sql/parser/value.h"
#include "storage/index/index.h"
#include "storage/record/record.h"
#include "storage/trx/trx.h"
#include <numeric>

IndexScanPhysicalOperator::IndexScanPhysicalOperator(Table *table, Index *index, bool readonly,
    const std::vector<Value> &left_values, bool left_inclusive, const std::vector<Value> &right_values,
    bool right_inclusive, const std::vector<FieldMeta> &value_metas)
    : table_(table),
      index_(index),
      readonly_(readonly),
      left_values_(left_values),
      right_values_(right_values),
      value_metas_(value_metas),
      left_inclusive_(left_inclusive),
      right_inclusive_(right_inclusive)
{}

RC IndexScanPhysicalOperator::open(Trx *trx)
{
  if (nullptr == table_ || nullptr == index_) {
    return RC::INTERNAL;
  }

  auto make_key = [this](const std::vector<Value> &values) -> char * {
    // 注意: 要附加上相应的bitmap

    // 计算values的长度
    int total_length =
        std::accumulate(values.begin(), values.end(), 0, [](int sum, const Value &val) { return sum + val.length(); });

    // 附加上bitmap的长度
    const FieldMeta *null_field = this->table_->table_meta().null_field();
    total_length += null_field->len();

    char *key = new char[total_length];
    int offset{0};
    for (const Value &val : values) {
      memcpy(key + offset, val.data(), val.length());
      offset += val.length();
    }
    memset(key + offset, 0, null_field->len());
    common::Bitmap bitmap(key + offset, null_field->len());

    // 遍历 value 值, 设置对应 null 的 bitmap 为 1
    for (int i = 0; i < values.size(); ++i) {
      if (values[i].attr_type() == AttrType::NULLS) {
        bitmap.set_bit(value_metas_[i].id());
      }
    }

    return key;
  };

  char *left_key = make_key(left_values_);
  char *right_key = make_key(right_values_);

  IndexScanner *index_scanner = index_->create_scanner(
      left_key, left_values_[0].length(), left_inclusive_, right_key, right_values_[0].length(), right_inclusive_);

  delete[] left_key;
  delete[] right_key;

  if (nullptr == index_scanner) {
    LOG_WARN("failed to create index scanner");
    return RC::INTERNAL;
  }

  record_handler_ = table_->record_handler();
  if (nullptr == record_handler_) {
    LOG_WARN("invalid record handler");
    index_scanner->destroy();
    return RC::INTERNAL;
  }
  index_scanner_ = index_scanner;

  tuple_.set_schema(table_, table_->table_meta().field_metas());

  trx_ = trx;
  return RC::SUCCESS;
}

RC IndexScanPhysicalOperator::next()
{
  RID rid;
  RC rc = RC::SUCCESS;

  record_page_handler_.cleanup();

  bool filter_result = false;
  while (RC::SUCCESS == (rc = index_scanner_->next_entry(&rid))) {
    rc = record_handler_->get_record(record_page_handler_, &rid, readonly_, &current_record_);
    if (rc != RC::SUCCESS) {
      return rc;
    }

    tuple_.set_record(&current_record_);
    rc = filter(tuple_, filter_result);
    if (rc != RC::SUCCESS) {
      return rc;
    }

    if (!filter_result) {
      continue;
    }

    rc = trx_->visit_record(table_, current_record_, readonly_);
    if (rc == RC::RECORD_INVISIBLE) {
      continue;
    } else {
      return rc;
    }
  }

  return rc;
}

RC IndexScanPhysicalOperator::close()
{
  index_scanner_->destroy();
  index_scanner_ = nullptr;
  return RC::SUCCESS;
}

Tuple *IndexScanPhysicalOperator::current_tuple()
{
  tuple_.set_record(&current_record_);
  return &tuple_;
}

void IndexScanPhysicalOperator::set_predicates(std::vector<std::unique_ptr<Expression>> &&exprs)
{
  predicates_ = std::move(exprs);
}

RC IndexScanPhysicalOperator::filter(RowTuple &tuple, bool &result)
{
  RC rc = RC::SUCCESS;
  Value value;
  for (std::unique_ptr<Expression> &expr : predicates_) {
    rc = expr->get_value(tuple, value);
    if (rc != RC::SUCCESS) {
      return rc;
    }

    bool tmp_result = value.get_boolean();
    if (!tmp_result) {
      result = false;
      return rc;
    }
  }

  result = true;
  return rc;
}

std::string IndexScanPhysicalOperator::param() const
{
  return std::string(index_->index_meta().name()) + " ON " + table_->name();
}
