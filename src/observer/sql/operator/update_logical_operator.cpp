#include "sql/operator/update_logical_operator.h"

UpdateLogicalOperator::UpdateLogicalOperator(Table *table, std::vector<std::unique_ptr<Expression>> &&out_value_exprs,
    const std::vector<const FieldMeta *> &field_metas)
    : value_exprs(std::move(out_value_exprs)), table_(table), field_metas_(field_metas)
{}
