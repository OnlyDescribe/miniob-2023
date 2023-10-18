#include "sql/operator/update_logical_operator.h"

UpdateLogicalOperator::UpdateLogicalOperator(
    Table *table, const Value *values, const FieldMeta *field_metas, int value_amount)
    : table_(table), values_(values), field_metas_(field_metas), value_amount_(value_amount)
{}
