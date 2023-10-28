#include "common/log/log.h"
#include "sql/operator/update_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/stmt/update_stmt.h"

UpdatePhysicalOperator::UpdatePhysicalOperator(
    Table *table, const std::vector<const Value *> &values, const std::vector<const FieldMeta *> &field_metas)
    : table_(table), values_(values), field_metas_(field_metas)
{
  // TODO(oldcb): 目前可以被field的 id 值替代
  // 找到需要修改的字段是第几个
  index_field_metas_.reserve(field_metas_.size());
  for (const auto &field_meta : field_metas_) {
    int field_index;
    const std::vector<FieldMeta> &table_field_metas = *table_->table_meta().field_metas();
    auto it = std::find_if(
        table_field_metas.begin(), table_field_metas.end(), [update_field_meta = field_meta](const FieldMeta &meta) {
          if (strcmp(meta.name(), update_field_meta->name()) == 0) {
            return true;
          }
          return false;
          // return meta.name() == update_field_meta->name();
        });

    ASSERT(it != table_field_metas.end(), "failed to get field index");
    field_index = std::distance(table_field_metas.begin(), it);
    field_index -= table_->table_meta().sys_field_num();  // 需要考虑sys_field
    index_field_metas_.push_back(field_index);
  }
}

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

  int sys_field_num = table_->table_meta().sys_field_num();

  // next
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    // 得到需要删除的Tuple
    RowTuple *row_tuple = static_cast<RowTuple *>(tuple);
    Record &old_record = row_tuple->record();

    // 替换得到新Tuple
    Record new_record;
    Value cell;

    std::vector<Value> values;
    int value_num = row_tuple->cell_num();
    values.reserve(value_num);

    for (int i = 0, j = 0; i < value_num; ++i) {
      if (index_field_metas_[j] == i) {
        values.push_back(*values_[j]);
        ++j;
      } else {
        row_tuple->cell_at(i, cell);
        values.push_back(cell);
      }
    }

    RC rc = table_->make_record(value_num, values.data(), new_record);

    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to make record. rc=%s", strrc(rc));
      // TODO(oldcb): 应该要把可能的溢出页的资源删除
      return rc;
    }

    // 更新Tuple
    rc = trx_->update_record(table_, old_record, new_record);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to update record by transaction. rc=%s", strrc(rc));
      return rc;
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
