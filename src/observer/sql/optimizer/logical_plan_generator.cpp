/* Copyright (c) 2023 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2023/08/16.
//

#include "sql/optimizer/logical_plan_generator.h"

#include "sql/operator/logical_operator.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/orderby_logical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/aggregation_logical_operator.h"
#include "sql/operator/update_logical_operator.h"

#include "sql/stmt/stmt.h"
#include "sql/stmt/calc_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "sql/stmt/filter_stmt.h"
#include "sql/stmt/insert_stmt.h"
#include "sql/stmt/delete_stmt.h"
#include "sql/stmt/update_stmt.h"
#include "sql/stmt/explain_stmt.h"

using namespace std;

RC LogicalPlanGenerator::create(Stmt *stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  switch (stmt->type()) {
    case StmtType::CALC: {
      CalcStmt *calc_stmt = static_cast<CalcStmt *>(stmt);
      rc = create_plan(calc_stmt, logical_operator);
    } break;

    case StmtType::SELECT: {
      SelectStmt *select_stmt = static_cast<SelectStmt *>(stmt);
      rc = create_plan(select_stmt, logical_operator);
    } break;

    case StmtType::INSERT: {
      InsertStmt *insert_stmt = static_cast<InsertStmt *>(stmt);
      rc = create_plan(insert_stmt, logical_operator);
    } break;

    case StmtType::DELETE: {
      DeleteStmt *delete_stmt = static_cast<DeleteStmt *>(stmt);
      rc = create_plan(delete_stmt, logical_operator);
    } break;

    case StmtType::UPDATE: {
      UpdateStmt *update_stmt = static_cast<UpdateStmt *>(stmt);
      rc = create_plan(update_stmt, logical_operator);
    } break;

    case StmtType::EXPLAIN: {
      ExplainStmt *explain_stmt = static_cast<ExplainStmt *>(stmt);
      rc = create_plan(explain_stmt, logical_operator);
    } break;
    default: {
      rc = RC::UNIMPLENMENT;
    }
  }
  return rc;
}

RC LogicalPlanGenerator::create_plan(CalcStmt *calc_stmt, std::unique_ptr<LogicalOperator> &logical_operator)
{
  logical_operator.reset(new CalcLogicalOperator(std::move(calc_stmt->expressions())));
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(SelectStmt *select_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  unique_ptr<LogicalOperator> table_oper(nullptr);

  const std::vector<Table *> &tables = select_stmt->tables();
  const std::vector<Field> &all_fields = select_stmt->query_fields();
  // 连表条件
  const auto &join_on_units = select_stmt->join_on_stmt()->join_units();

  if (!join_on_units.empty() && join_on_units.size() + 1 != tables.size()) {
    return RC::JOIN_ERROR;
  }
  int join_on_units_index = 0;

  for (Table *table : tables) {
    std::vector<Field> fields;
    for (const Field &field : all_fields) {
      if (0 == strcmp(field.table_name(), table->name())) {
        fields.push_back(field);
      }
    }

    unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, true /*readonly*/));
    if (table_oper == nullptr) {
      table_oper = std::move(table_get_oper);
    } else {
      JoinLogicalOperator *join_oper = new JoinLogicalOperator;
      join_oper->add_child(std::move(table_oper));
      join_oper->add_child(std::move(table_get_oper));
      table_oper = unique_ptr<LogicalOperator>(join_oper);
      // 连表条件设置为hashjoin, 里面的连接条件都是AND, OR 暂时不考虑
      if (join_on_units_index < join_on_units.size()) {
        auto &units = join_on_units[join_on_units_index];
        std::vector<unique_ptr<Expression>> cmp_exprs;
        for (int i = 0; i < units.size(); i++) {
          const JoinOnObj &join_on_obj_left = units[i]->left();
          const JoinOnObj &join_on_obj_right = units[i]->right();
          unique_ptr<Expression> left(join_on_obj_left.is_attr
                                          ? static_cast<Expression *>(new FieldExpr(join_on_obj_left.field))
                                          : static_cast<Expression *>(new ValueExpr(join_on_obj_left.value)));

          unique_ptr<Expression> right(join_on_obj_right.is_attr
                                           ? static_cast<Expression *>(new FieldExpr(join_on_obj_right.field))
                                           : static_cast<Expression *>(new ValueExpr(join_on_obj_right.value)));

          ComparisonExpr *cmp_expr = new ComparisonExpr(units[i]->comp(), std::move(left), std::move(right));
          cmp_exprs.emplace_back(static_cast<Expression *>(cmp_expr));
        }
        join_oper->set_expressions(std::move(cmp_exprs));
        join_on_units_index++;
      }
      join_oper->right_table = table;
    }
  }

  unique_ptr<LogicalOperator> predicate_oper;
  RC rc = create_plan(select_stmt->filter_stmt(), predicate_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create predicate logical plan. rc=%s", strrc(rc));
    return rc;
  }

  // 自低向上，构建logic_tree, 走完这里op是一个全表扫或者join
  unique_ptr<LogicalOperator> top_op;
  if (predicate_oper) {
    if (table_oper) {
      predicate_oper->add_child(std::move(table_oper));
    }
    top_op = std::move(predicate_oper);
  } else {
    top_op = std::move(table_oper);
  }

  // orderby
  unique_ptr<LogicalOperator> orderby_oper;
  rc = create_plan(select_stmt->orderbys(), orderby_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create orderby logical plan. rc=%s", strrc(rc));
    return rc;
  }
  if (orderby_oper) {
    orderby_oper->add_child(std::move(top_op));
    top_op.swap(orderby_oper);
  }

  // 聚合 logic_op，直接使用构造
  if (select_stmt->is_aggregation_stmt()) {
    unique_ptr<LogicalOperator> aggr_oper = make_unique<AggregationLogicalOperator>();
    std::vector<std::unique_ptr<Expression>> expressions;
    for (const auto &field : all_fields) {
      if (field.with_aggr()) {
        expressions.emplace_back(std::make_unique<AggretationExpr>(field, field.get_aggr_type()));
      }
    }
    static_cast<AggregationLogicalOperator *>(aggr_oper.get())->set_aggregation_expr(std::move(expressions));
    aggr_oper->add_child(std::move(top_op));
    top_op.swap(aggr_oper);
  } else {
    unique_ptr<LogicalOperator> project_oper(new ProjectLogicalOperator(all_fields));
    if (top_op) {
      project_oper->add_child(std::move(top_op));
    }
    top_op.swap(project_oper);
  }

  logical_operator.swap(top_op);
  return RC::SUCCESS;
}

/**
 * @description: 创造查询过滤条件的算子
 * @param {FilterStmt} *filter_stmt
 * @param {unique_ptr<LogicalOperator>} &logical_operator
 */
RC LogicalPlanGenerator::create_plan(FilterStmt *filter_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  RC rc = RC::SUCCESS;
  std::vector<unique_ptr<Expression>> cmp_exprs;
  const std::vector<FilterUnit *> &filter_units = filter_stmt->filter_units();
  for (FilterUnit *filter_unit : filter_units) {
    // 创建filter_unit左右子查询的逻辑算子
    if (filter_unit->left->type() == ExprType::SUBQUERY) {
      auto sub_query_expr = static_cast<SubQueryExpr *>(filter_unit->left.get());
      if (sub_query_expr->subquery_stmt->query_fields().size() != 1) {
        return RC::SUBQUERY_MANY_COLS;
      }
      rc = create_plan(sub_query_expr->subquery_stmt, sub_query_expr->oper);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
    if (filter_unit->right->type() == ExprType::SUBQUERY) {
      auto sub_query_expr = static_cast<SubQueryExpr *>(filter_unit->right.get());
      if (sub_query_expr->subquery_stmt->query_fields().size() != 1) {
        return RC::SUBQUERY_MANY_COLS;
      }
      rc = create_plan(sub_query_expr->subquery_stmt, sub_query_expr->oper);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
    ComparisonExpr *cmp_expr =
        new ComparisonExpr(filter_unit->comp, std::move(filter_unit->left), std::move(filter_unit->right));
    cmp_exprs.emplace_back(cmp_expr);
  }

  unique_ptr<PredicateLogicalOperator> predicate_oper;
  if (!cmp_exprs.empty()) {
    unique_ptr<ConjunctionExpr> conjunction_expr(new ConjunctionExpr(ConjunctionExpr::Type::AND, cmp_exprs));
    predicate_oper = unique_ptr<PredicateLogicalOperator>(new PredicateLogicalOperator(std::move(conjunction_expr)));
  }

  logical_operator = std::move(predicate_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(InsertStmt *insert_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table *table = insert_stmt->table();
  vector<Value> values(insert_stmt->values(), insert_stmt->values() + insert_stmt->value_amount());
  // 避免多一次的拷贝, 但还是有一次拷贝
  // TODO(oldcb): 性能优化, 避免values值拷贝
  InsertLogicalOperator *insert_operator = new InsertLogicalOperator(table, std::move(values));
  logical_operator.reset(insert_operator);
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(DeleteStmt *delete_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Table *table = delete_stmt->table();
  FilterStmt *filter_stmt = delete_stmt->filter_stmt();
  std::vector<Field> fields;
  int field_num = table->table_meta().field_num() - table->table_meta().extra_field_num();
  for (int i = table->table_meta().sys_field_num(); i < field_num; i++) {
    const FieldMeta *field_meta = table->table_meta().field(i);
    fields.push_back(Field(table, field_meta));
  }
  unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, false /*readonly*/));

  unique_ptr<LogicalOperator> predicate_oper;
  RC rc = create_plan(filter_stmt, predicate_oper);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  unique_ptr<LogicalOperator> delete_oper(new DeleteLogicalOperator(table));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    delete_oper->add_child(std::move(predicate_oper));
  } else {
    delete_oper->add_child(std::move(table_get_oper));
  }

  logical_operator = std::move(delete_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(
    std::vector<OrderByUnit> *orderbys, std::unique_ptr<LogicalOperator> &logical_operator)
{
  std::vector<SortType> sort_types;
  std::vector<std::unique_ptr<FieldExpr>> field_exprs;
  for (int i = 0; i < orderbys->size(); i++) {
    field_exprs.emplace_back(new FieldExpr((*orderbys)[i].field));
    sort_types.emplace_back((*orderbys)[i].sort_type);
  }
  if (!field_exprs.empty()) {
    logical_operator = std::make_unique<OrderByLogicalOperator>(std::move(field_exprs), std::move(sort_types));
  }
  return RC::SUCCESS;
}

RC LogicalPlanGenerator::create_plan(UpdateStmt *update_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  // 类似 DELETE
  Table *table = update_stmt->table();
  FilterStmt *filter_stmt = update_stmt->filter_stmt();

  std::vector<Field> fields;
  int field_num = table->table_meta().field_num() - table->table_meta().extra_field_num();
  for (int i = table->table_meta().sys_field_num(); i < field_num; i++) {
    const FieldMeta *field_meta = table->table_meta().field(i);
    fields.push_back(Field(table, field_meta));
  }
  unique_ptr<LogicalOperator> table_get_oper(new TableGetLogicalOperator(table, fields, false /*readonly*/));

  unique_ptr<LogicalOperator> predicate_oper;
  RC rc = create_plan(filter_stmt, predicate_oper);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  const std::vector<const Value *> &values_ = update_stmt->values();
  const std::vector<const FieldMeta *> &field_metas_ = update_stmt->field_metas();

  unique_ptr<LogicalOperator> update_oper(new UpdateLogicalOperator(table, values_, field_metas_));

  if (predicate_oper) {
    predicate_oper->add_child(std::move(table_get_oper));
    update_oper->add_child(std::move(predicate_oper));
  } else {
    update_oper->add_child(std::move(table_get_oper));
  }

  logical_operator = std::move(update_oper);
  return rc;
}

RC LogicalPlanGenerator::create_plan(ExplainStmt *explain_stmt, unique_ptr<LogicalOperator> &logical_operator)
{
  Stmt *child_stmt = explain_stmt->child();
  unique_ptr<LogicalOperator> child_oper;
  RC rc = create(child_stmt, child_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create explain's child operator. rc=%s", strrc(rc));
    return rc;
  }

  logical_operator = unique_ptr<LogicalOperator>(new ExplainLogicalOperator);
  logical_operator->add_child(std::move(child_oper));
  return rc;
}
