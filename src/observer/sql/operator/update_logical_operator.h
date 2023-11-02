#pragma once

#include "sql/operator/logical_operator.h"

class UpdateLogicalOperator : public LogicalOperator
{
public:
  UpdateLogicalOperator(Table *table, std::vector<std::unique_ptr<Expression>> &&values,
      const std::vector<const FieldMeta *> &field_metas);
  virtual ~UpdateLogicalOperator() = default;

  LogicalOperatorType type() const override { return LogicalOperatorType::UPDATE; }
  Table *table() const { return table_; }
  const auto &field_metas() const { return field_metas_; }

  std::vector<std::unique_ptr<Expression>> value_exprs;

private:
  Table *table_ = nullptr;
  std::vector<const FieldMeta *> field_metas_;
};
