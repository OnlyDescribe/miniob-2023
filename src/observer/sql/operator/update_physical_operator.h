#pragma once

#include "sql/operator/physical_operator.h"

class Trx;

class UpdatePhysicalOperator : public PhysicalOperator
{
public:
  UpdatePhysicalOperator(
      Table *table, const std::vector<const Value *> &values, const std::vector<const FieldMeta *> &field_metas);

  virtual ~UpdatePhysicalOperator() = default;

  PhysicalOperatorType type() const override { return PhysicalOperatorType::UPDATE; }

  RC open(Trx *trx) override;
  RC next() override;
  RC close() override;

  Tuple *current_tuple() override { return nullptr; }

private:
  Table *table_ = nullptr;
  std::vector<const Value *> values_;
  std::vector<const FieldMeta *> field_metas_;
  std::vector<int> index_field_metas_;

  Trx *trx_ = nullptr;
};
