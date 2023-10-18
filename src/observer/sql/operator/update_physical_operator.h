#pragma once

#include "sql/operator/physical_operator.h"

class Trx;

class UpdatePhysicalOperator : public PhysicalOperator
{
public:
  UpdatePhysicalOperator(Table *table, const Value *values, const FieldMeta *field_metas, int value_amount)
      : table_(table), values_(values), field_metas_(field_metas), value_amount_(value_amount)
  {}

  virtual ~UpdatePhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::UPDATE; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override { return nullptr; }

private:
  Table *table_ = nullptr;
  const Value *values_ = nullptr;
  const FieldMeta *field_metas_ = nullptr;
  int value_amount_ = 0;

  Trx *trx_ = nullptr;
};
