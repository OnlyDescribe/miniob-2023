#pragma once

#include "sql/operator/physical_operator.h"

class Trx;

class UpdatePhysicalOperator : public PhysicalOperator
{
public:
  UpdatePhysicalOperator(Table *table, std::vector<std::unique_ptr<Expression>> &&values_exprs,
      const std::vector<const FieldMeta *> &field_metas);

  virtual ~UpdatePhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::UPDATE; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override { return nullptr; }

private:
  Table *table_ = nullptr;
  std::vector<std::unique_ptr<Expression>> values_exprs_;
  std::vector<Value> values_;
  std::vector<const FieldMeta *> field_metas_;
  std::vector<int> index_field_metas_;       // 需要修改的字段是第几个, 不考虑系统字段

  bool schema_field_type_mismatch_ = false;  // 是否字段和修改值的类型不一致

  Trx *trx_ = nullptr;
};
