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
// Created by WangYunlai on 2021/6/9.
//

#include "sql/operator/insert_physical_operator.h"
#include "sql/parser/value.h"
#include "sql/stmt/insert_stmt.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include <cstring>
#include <vector>

using namespace std;

InsertPhysicalOperator::InsertPhysicalOperator(Table *table, vector<Value> &&values)
    : table_(table), values_(std::move(values))
{}

RC InsertPhysicalOperator::open(Trx *trx)
{
  RC rc = RC::SUCCESS;
  // 若是视图
  if (table_->is_view()) {
    // Insert为每个表所未指定的值会补充NULL
    // 对于Join的视图, 需要指定Insert的列名
    // if (!table_->modifiable()) {
    //   LOG_WARN("failed to delete record: %s");
    //   return RC::INVALID_ARGUMENT;
    // }

    const TableMeta &view_meta = table_->table_meta();
    for (int i = 0; i < table_->table_meta().view_tables().size(); ++i) {
      const Table *origin_table = table_->table_meta().view_table(i);
      const TableMeta &origin_table_meta = origin_table->table_meta();

      const std::vector<FieldMeta> view_field_metas = *view_meta.field_metas();
      const std::vector<FieldMeta> table_field_metas = *origin_table_meta.field_metas();

      // 组装需要插入的值, 如果没有指定则设置NULL, 如果不可设置为默认, 应该Failure
      int origin_table_field_num =
          origin_table_meta.field_num() - origin_table_meta.extra_field_num() - origin_table_meta.sys_field_num();
      std::vector<Value> values;
      values.reserve(origin_table_field_num);
      for (int i = 0, j = 0; i < origin_table_field_num; ++i) {
        if (strcmp(table_field_metas[i].name(), view_field_metas[j].name()) == 0) {
          values.push_back(values_[j]);
          ++j;
        } else {
          Value null;
          null.set_null();
          values.push_back(null);
        }
      }

      Record record;
      rc = const_cast<Table *>(origin_table)->make_record(static_cast<int>(values.size()), values.data(), record);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to make record. rc=%s", strrc(rc));
        return rc;
      }

      rc = trx->insert_record(const_cast<Table *>(origin_table), record);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to insert record by transaction. rc=%s", strrc(rc));
      }

      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to insert record by transaction. rc=%s", strrc(rc));
      }
    }

    return rc;
  }

  Record record;
  rc = table_->make_record(static_cast<int>(values_.size()), values_.data(), record);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to make record. rc=%s", strrc(rc));
    return rc;
  }

  rc = trx->insert_record(table_, record);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to insert record by transaction. rc=%s", strrc(rc));
  }
  return rc;
}

RC InsertPhysicalOperator::next() { return RC::RECORD_EOF; }

RC InsertPhysicalOperator::close() { return RC::SUCCESS; }
