#include "sql/operator/update_logical_operator.h"

UpdateLogicalOperator::UpdateLogicalOperator(
    Table *table, const std::vector<const Value *> &values, const std::vector<const FieldMeta *> &field_metas)
    : table_(table), values_(values), field_metas_(field_metas)
{}
