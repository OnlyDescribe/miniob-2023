#include "common/log/log.h"
#include "common/rc.h"
#include "sql/expr/tuple.h"
#include "sql/operator/update_physical_operator.h"
#include "storage/record/record.h"
#include "storage/table/table.h"
#include "storage/trx/trx.h"
#include "sql/stmt/update_stmt.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>
#include <vector>

UpdatePhysicalOperator::UpdatePhysicalOperator(Table *table, std::vector<std::unique_ptr<Expression>> &&values_exprs,
    const std::vector<const FieldMeta *> &field_metas)
    : table_(table), values_exprs_(std::move(values_exprs)), field_metas_(field_metas)
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
  for (int i = 0; i < values_exprs_.size(); i++) {
    // 注意: 如果这里判断字段和值类型无法类型转换或是子查询返回多行, 先不能直接报错
    // 而是要等到next中扫表的时候, 如果没有拿到需要更改的tuple值, 则不报错

    Value value;

    if (values_exprs_[i]->type() == ExprType::VALUE) {
      auto value_expr = static_cast<ValueExpr *>(values_exprs_[i].get());
      value_expr->get_value(value);
    } else if (values_exprs_[i]->type() == ExprType::SUBQUERY) {
      auto sub_query_expr = static_cast<SubQueryExpr *>(values_exprs_[i].get());
      // 只支持简单子查询, 先不支持和其他查询联动
      rc = sub_query_expr->get_one_row_value(RowTuple(), value);
      if (rc != RC::SUCCESS) {
        schema_field_type_mismatch_ = true;
        break;
      }
    } else {
      return RC::NOT_IMPLEMENT;
    }

    // 尽可能解决Unary表达式与字段类型不统一的冲突
    const FieldMeta *field_meta = field_metas_[i];
    AttrType field_type = field_meta->type();
    AttrType value_type = value.attr_type();
    if (field_type != value_type) {

      // 1) 因为更新操作的词法解析无法判断字符串是TEXTS还是CHARS
      // 目前可能会出现值 TEXTS 类型而字段是 CHARS 类型
      if (field_type == AttrType::TEXTS && value_type == AttrType::CHARS) {
        value.set_type(AttrType::TEXTS);
      }
      // 2) 如果值为 NULL, 判断该字段是否设置了 NOT NULL
      else if (value_type == AttrType::NULLS) {
        if (field_meta->is_not_null()) {
          schema_field_type_mismatch_ = true;
          break;
        }
      }
      // 3) 处理INTS, FLOATS和CHARS之间的类型转换
      else if ((field_type == AttrType::INTS || field_type == AttrType::FLOATS || field_type == AttrType::CHARS) and
               (value_type == AttrType::INTS || value_type == AttrType::FLOATS || value_type == AttrType::CHARS)) {
        switch (field_type) {
            // 注意: FLOATS 要截断值; CHARS 若是纯数字同样阶段转换值, 否则报错
          case AttrType::INTS: {
            // 注意: 转INTS要四舍五入
            int v = value.get_int(false);
            if (v != std::numeric_limits<int>::max()) {
              value = Value(v);
            } else {
            }
          } break;
          case AttrType::FLOATS: {
            int v = value.get_float(false);
            if (v != std::numeric_limits<float>::max()) {
              value = Value(v);
            } else {
              schema_field_type_mismatch_ = true;
            }
          } break;
          case AttrType::CHARS: {
            value = Value(value.get_string().data());
          } break;
          default: {
            schema_field_type_mismatch_ = true;
          } break;
        }
      }
    }

    if (schema_field_type_mismatch_) {
      break;
    }

    values_.emplace_back(value);
  }

  return RC::SUCCESS;
}

RC UpdatePhysicalOperator::next()
{
  RC rc = RC::SUCCESS;

  PhysicalOperator *child = children_[0].get();

  // next
  while (RC::SUCCESS == (rc = child->next())) {
    Tuple *tuple = child->current_tuple();
    if (nullptr == tuple) {
      LOG_WARN("failed to get current record: %s", strrc(rc));
      return rc;
    }

    if (schema_field_type_mismatch_ == true) {
      LOG_WARN("field type mismatch. table=%s", table_->name());
      return RC::SCHEMA_FIELD_TYPE_MISMATCH;
    }

    // 如果是视图, 更新的tuple不一定是RowTuple
    if (table_->is_view()) {
      if (!table_->updatable()) {
        LOG_WARN("can not be updated");
        return RC::INVALID_ARGUMENT;
      }
      const TableMeta &table_meta = table_->table_meta();
      const std::vector<const Table *> view_tables = table_meta.view_tables();

      // 需要找到视图中需要修改的原表, 以及相应的原字段元信息, 修改值，需要更新的record的rid
      struct IdValuePair
      {
        int id;
        Value value;
      };
      struct UpdateMeta  // 暂时先直接复制
      {
        RecordPos rid;  // 原表中需要更新的record的rid
        // std::vector<FieldMeta> field_metas;  // 原字段元信息
        // std::vector<Value> values;           // 修改值

        std::vector<IdValuePair> id_values;
      };

      std::map<const Table *, UpdateMeta> table_update_map;

      for (int i = 0; i < field_metas_.size(); i++) {
        const FieldMeta *meta = field_metas_[i];
        Value &value = values_[i];

        // 找到视图字段对应的原表
        const Table *origin_table = view_tables[meta->id()];

        // 原表的更新信息
        UpdateMeta &update_meta = table_update_map[origin_table];

        IdValuePair id_value_pair;
        // update_meta.values.push_back(value);
        id_value_pair.value = value;

        // 将更新元信息与原表进行对应
        const std::vector<FieldMeta> &origin_field_metas = *origin_table->table_meta().field_metas();
        for (const FieldMeta &origin_meta : origin_field_metas) {
          assert(origin_meta.name() != nullptr);
          assert(meta->name() != nullptr);
          if (strcmp(origin_meta.name(), meta->name()) == 0) {
            // update_meta.field_metas.push_back(origin_meta);
            // 找到原表的字段元信息
            id_value_pair.id = origin_meta.id();

            // 并找到原表要更新的record的id
            std::string alias;  // 暂时用不到
            rc = tuple->find_record(TupleCellSpec(table_->name(), origin_meta.name()), update_meta.rid);
            if (rc != RC::SUCCESS) {
              return rc;
            }
            break;
          }
        }
        table_update_map[origin_table].id_values.push_back(std::move(id_value_pair));
      }

      // 执行更新
      for (auto &&[origin_table, update_meta] : table_update_map) {
        const TableMeta &origin_table_meta = origin_table->table_meta();

        Record old_record;
        RowTuple old_tuple;
        RID rid = update_meta.rid.rid;
        std::vector<IdValuePair> &id_value_pairs = update_meta.id_values;

        std::sort(id_value_pairs.begin(), id_value_pairs.end(), [](const IdValuePair &lhs, const IdValuePair &rhs) {
          return lhs.id < rhs.id;
        });

        const_cast<Table *>(origin_table)->get_record(rid, old_record);

        old_tuple.set_table(origin_table);
        old_tuple.set_schema(origin_table, origin_table->table_meta().field_metas());
        old_tuple.set_record(&old_record);

        // 替换得到新Tuple
        Record new_record;
        Value cell;

        std::vector<Value> values;
        int value_num =
            origin_table_meta.field_num() - origin_table_meta.extra_field_num() - origin_table_meta.sys_field_num();
        values.reserve(value_num);

        for (int i = 0, j = 0; i < value_num; ++i) {
          if (id_value_pairs[j].id == i) {
            values.push_back(id_value_pairs[j].value);
            ++j;
          } else {
            old_tuple.cell_at(i, cell);
            values.push_back(cell);
          }
        }

        RC rc = const_cast<Table *>(origin_table)->make_record(value_num, values.data(), new_record);

        if (rc != RC::SUCCESS) {
          LOG_WARN("failed to make record. rc=%s", strrc(rc));
          // TODO(oldcb): 应该要把可能的溢出页的资源删除
          return rc;
        }

        // 更新Tuple
        rc = trx_->update_record(const_cast<Table *>(origin_table), old_record, new_record);
        if (rc != RC::SUCCESS) {
          LOG_WARN("failed to update record by transaction. rc=%s", strrc(rc));
          return rc;
        }
      }

      continue;
    }

    // 2. 如果不是视图, 更新的tuple一定是RowTuple
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
        values.push_back(values_[j]);
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
