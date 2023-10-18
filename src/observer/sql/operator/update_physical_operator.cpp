#include "common/log/log.h"
#include "sql/operator/update_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/stmt/update_stmt.h"

RC UpdatePhysicalOperator::open(Trx *trx)
{
  ASSERT(children_.size() == 1, "update operator must has one child");

  std::unique_ptr<PhysicalOperator> &child = children_[0];
  RC rc = child->open(trx);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to open child operator: %s", strrc(rc));
    return rc;
  }

  trx_ = trx;

  // TODO(oldcb): 处理多个交的表达式(如子查询), 生成子查询的physical operator, 并执行返回表达式的值
  // 目前只有一个字段和相应的值

  // TODO(oldcb): 多字段
  // TODO(oldcb): 支持 NULL
  // TODO(oldcb): 支持 类型转换
  //   const AttrType value_type = values_->attr_type();
  //   const AttrType field_type = field_metas_->type();

  return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;

  PhysicalOperator *child = children_[0].get();

  // 找到需要修改的字段是第几个
  int field_index;
  const std::vector<FieldMeta> &field_metas_vec = *table_->table_meta().field_metas();
  auto it = std::find_if(field_metas_vec.begin(),
      field_metas_vec.end(),
      [field_metas_ = this->field_metas_](const FieldMeta &meta) { return meta.name() == field_metas_->name(); });

  ASSERT(it != field_metas_vec.end(), "failed to get field index");
  field_index = std::distance(field_metas_vec.begin(), it);
  field_index -= table_->table_meta().sys_field_num();

  // next
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    // 得到旧Tuple
    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);

    // 删除旧Tuple
    Record &old_record = row_tuple->record();
    rc = trx_->delete_record(table_, old_record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to delete record: %s", strrc(rc));
      return rc;
    }

    // 替换得到新Tuple
    Record new_record;

    std::vector<Value> values;
    Value cell;
    int value_num = row_tuple->cell_num();
    values.reserve(value_num);
    for (int i = 0; i < value_num; ++i) {
      row_tuple->cell_at(i, cell);
      if (field_index == i) {
        values.push_back(*values_);
      } else {
        values.push_back(cell);
      }
    }
    RC rc = table_->make_record(value_num, values.data(), new_record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to make record. rc=%s", strrc(rc));
      return rc;
    }

    // 插入新Tuple
    rc = trx_->insert_record(table_, new_record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to insert record by transaction. rc=%s", strrc(rc));
    }
  }

  return RC::RECORD_EOF;
}

RC UpdatePhysicalOperator::close()
{
  if (!children_.empty()) {
    children_[0]->close();
  }
  return RC::SUCCESS;
}
