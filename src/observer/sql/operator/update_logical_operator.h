#pragma once

#include "sql/operator/logical_operator.h"

class UpdateLogicalOperator : public LogicalOperator
{
public:
  UpdateLogicalOperator(
      Table *table, const std::vector<const Value *> &values, const std::vector<const FieldMeta *> &field_metas);
  virtual ~UpdateLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::UPDATE; }
  Table *table() const { return table_; }
  const auto &values() const { return values_; }
  const auto &field_metas() const { return field_metas_; }
  int value_amount() const { return values_.size(); }

private:
  Table *table_ = nullptr;

  std::vector<const Value *> values_;
  std::vector<const FieldMeta *> field_metas_;
};
