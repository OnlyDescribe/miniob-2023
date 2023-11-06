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
// Created by WangYunlai on 2022/6/27.
//

#include "common/log/log.h"
#include "sql/operator/delete_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/stmt/delete_stmt.h"

RC DeletePhysicalOperator::open(Trx *trx)
{
  if (children_.empty()) {
    return RC::SUCCESS;
  }

  std::unique_ptr<PhysicalOperator> &child = children_[0];
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  trx_ = trx;

  return RC::SUCCESS;
}

RC DeletePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    return RC::RECORD_EOF;
  }

  PhysicalOperator *child = children_[0].get();
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    // 若是视图
    if (table_->is_view()) {
      if (!table_->modifiable()) {
        // 删除应该只支持单表
        LOG_WARN("failed to delete record: %s");
        return RC::INVALID_ARGUMENT;
      }
      const Table *origin_table = table_->table_meta().view_table(0);
      // const TableMeta &origin_table_meta = origin_table->table_meta();

      // 并找到原表要删除的record的id
      std::string alias;  // 暂时用不到
      RecordPos rid;
      rc = tuple->find_record(
          TupleCellSpec(table_->name(), table_->table_meta().field(table_->table_meta().sys_field_num())->name()), rid);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      Record old_record;
      const_cast<Table *>(origin_table)->get_record(rid.rid, old_record);

      rc = trx_->delete_record(const_cast<Table *>(origin_table), old_record);
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to delete record: %s", strrc(rc));
        return rc;
      }
      continue;
    }

    // 若不是视图
    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record &record = row_tuple->record();
    rc = trx_->delete_record(table_, record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to delete record: %s", strrc(rc));
      return rc;
    }
  }

  return RC::RECORD_EOF;
}

RC DeletePhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}
