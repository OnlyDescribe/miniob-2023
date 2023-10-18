#pragma once

#include "sql/operator/logical_operator.h"

class UpdateLogicalOperator : public LogicalOperator
{
public:
  UpdateLogicalOperator(Table *table, const Value *values, const FieldMeta *field_metas, int value_amount);
  virtual ~UpdateLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::UPDATE; }
  Table *table() const { return table_; }
  const Value *values() const { return values_; }
  const FieldMeta *field_metas() const { return field_metas_; }
  int value_amount() const { return value_amount_; }

private:
  Table *table_ = nullptr;

  const Value *values_ = nullptr;
  const FieldMeta *field_metas_ = nullptr;
  int value_amount_ = 0;
};
