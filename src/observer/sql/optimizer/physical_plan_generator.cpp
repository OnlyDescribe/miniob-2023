/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

//
// Created by Wangyunlai on 2022/12/14.
//

#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

#include "common/rc.h"
#include "sql/operator/update_logical_operator.h"
#include "sql/operator/update_physical_operator.h"
#include "sql/operator/view_get_logical_operator.h"
#include "sql/optimizer/logical_plan_generator.h"
#include "sql/optimizer/optimize_stage.h"
#include "sql/optimizer/physical_plan_generator.h"
#include "sql/operator/table_get_logical_operator.h"
#include "sql/operator/table_scan_physical_operator.h"
#include "sql/operator/index_scan_physical_operator.h"
#include "sql/operator/predicate_logical_operator.h"
#include "sql/operator/predicate_physical_operator.h"
#include "sql/operator/aggregation_logical_operator.h"
#include "sql/operator/aggregation_physical_operator.h"
#include "sql/operator/groupby_logical_operator.h"
#include "sql/operator/groupby_physical_operator.h"
#include "sql/operator/orderby_logical_operator.h"
#include "sql/operator/orderby_physical_operator.h"
#include "sql/operator/project_logical_operator.h"
#include "sql/operator/project_physical_operator.h"
#include "sql/operator/insert_logical_operator.h"
#include "sql/operator/insert_physical_operator.h"
#include "sql/operator/delete_logical_operator.h"
#include "sql/operator/delete_physical_operator.h"
#include "sql/operator/explain_logical_operator.h"
#include "sql/operator/explain_physical_operator.h"
#include "sql/operator/join_logical_operator.h"
#include "sql/operator/join_physical_operator.h"
#include "sql/operator/calc_logical_operator.h"
#include "sql/operator/calc_physical_operator.h"
#include "sql/expr/expression.h"
#include "common/log/log.h"
#include "storage/field/field_meta.h"
#include "storage/table/table.h"

using namespace std;

RC PhysicalPlanGenerator::create(LogicalOperator &logical_operator, unique_ptr<PhysicalOperator> &oper)
{
  RC rc = RC::SUCCESS;

  switch (logical_operator.type()) {
    case LogicalOperatorType::CALC: {
      return create_plan(static_cast<CalcLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::TABLE_GET: {
      return create_plan(static_cast<TableGetLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::VIEW_GET: {
      return create_plan(static_cast<ViewGetLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::PREDICATE: {
      return create_plan(static_cast<PredicateLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::PROJECTION: {
      return create_plan(static_cast<ProjectLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::INSERT: {
      return create_plan(static_cast<InsertLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::DELETE: {
      return create_plan(static_cast<DeleteLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::UPDATE: {
      return create_plan(static_cast<UpdateLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::EXPLAIN: {
      return create_plan(static_cast<ExplainLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::JOIN: {
      return create_plan(static_cast<JoinLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::ORDER_BY: {
      return create_plan(static_cast<OrderByLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::AGGREGATION: {
      return create_plan(static_cast<AggregationLogicalOperator &>(logical_operator), oper);
    } break;

    case LogicalOperatorType::GROUPBY: {
      return create_plan(static_cast<GroupbyLogicalOperator &>(logical_operator), oper);
    } break;

    default: {
      return RC::INVALID_ARGUMENT;
    }
  }
  return rc;
}

RC PhysicalPlanGenerator::create_plan(ViewGetLogicalOperator &view_get_oper, unique_ptr<PhysicalOperator> &oper)
{
  Table *table = view_get_oper.table();

  // 1. 如果table是视图, 应该下面接视图的物理算子
  // 但是查询优化阶段的资源是unique_ptr的, 每次都被移动一点资源， 只能保存sql语句, 重建算子资源
  // 这里的策略是无比愚蠢的, 而无奈
  RC rc = RC::SUCCESS;
  Db *db = table->view_db();
  const std::string &view_sql = table->view_sql();

  ParsedSqlResult parsed_sql_result;
  parse(view_sql.c_str(), &parsed_sql_result);

  std::unique_ptr<ParsedSqlNode> sql_node = std::move(parsed_sql_result.sql_nodes().front());
  Stmt *view_stmt = nullptr;
  rc = Stmt::create_stmt(db, *sql_node, view_stmt);

  unique_ptr<LogicalOperator> view_logical_operator;
  LogicalPlanGenerator logical_plan_generator;
  OptimizeStage optimizer;
  rc = logical_plan_generator.create(view_stmt, view_logical_operator);
  if (rc != RC::SUCCESS) {
    if (rc != RC::UNIMPLENMENT) {
      LOG_WARN("failed to create logical plan. rc=%s", strrc(rc));
    }
    return rc;
  }
  rc = optimizer.rewrite(view_logical_operator);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to rewrite plan. rc=%s", strrc(rc));
    return rc;
  }

  rc = optimizer.optimize(view_logical_operator);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to optimize plan. rc=%s", strrc(rc));
    return rc;
  }

  std::unique_ptr<PhysicalOperator> phy_oper;
  rc = create(*view_logical_operator, phy_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create project logical operator's child physical operator. rc=%s", strrc(rc));
    return rc;
  }
  oper = std::move(phy_oper);

  return rc;
}

RC PhysicalPlanGenerator::create_plan(TableGetLogicalOperator &table_get_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<Expression>> &predicates = table_get_oper.predicates();
  // 看看是否有可以用于索引查找的表达式
  Table *table = table_get_oper.table();

  // 如果table不是视图, 正常选择table_scan/index_scan
  Index *index = nullptr;
  ValueExpr *value_expr = nullptr;

  struct FieldValueExpPair
  {
    Field field;
    ValueExpr *value;
  };

  // TODO(oldcb): 考虑过滤条件中出现多次相同字段

  // 根据values相应的字段的offset排序, 参考B+树的 KeyComparator
  // 因为索引判断key的大小的字段是按照表字段的顺序
  auto comp = [](const FieldValueExpPair &lhs, const FieldValueExpPair &rhs) {
    return lhs.field.meta()->offset() < rhs.field.meta()->offset();
  };
  std::set<FieldValueExpPair, decltype(comp)> field_valexp_pairs;
  std::vector<std::string> field_names;
  field_names.reserve(predicates.size());

  for (auto &expr : predicates) {
    if (expr->type() == ExprType::COMPARISON) {
      auto comparison_expr = static_cast<ComparisonExpr *>(expr.get());
      // 简单处理，就找等值查询
      if (comparison_expr->comp() != CompOp::EQUAL_TO) {
        continue;
      }

      unique_ptr<Expression> &left_expr = comparison_expr->left();
      unique_ptr<Expression> &right_expr = comparison_expr->right();
      // 左右比较的一边最少是一个值
      if (left_expr->type() != ExprType::VALUE && right_expr->type() != ExprType::VALUE) {
        continue;
      }

      FieldExpr *field_expr = nullptr;
      if (left_expr->type() == ExprType::FIELD) {
        ASSERT(right_expr->type() == ExprType::VALUE, "right expr should be a value expr while left is field expr");
        field_expr = static_cast<FieldExpr *>(left_expr.get());
        value_expr = static_cast<ValueExpr *>(right_expr.get());
      } else if (right_expr->type() == ExprType::FIELD) {
        ASSERT(left_expr->type() == ExprType::VALUE, "left expr should be a value expr while right is a field expr");
        field_expr = static_cast<FieldExpr *>(right_expr.get());
        value_expr = static_cast<ValueExpr *>(left_expr.get());
      }

      if (field_expr == nullptr) {
        continue;
      }

      const Field &field = field_expr->field();

      field_valexp_pairs.insert({field, value_expr});

      // 从过滤条件中, 生成按照表顺序的字段
      field_names.clear();
      std::transform(field_valexp_pairs.begin(),
          field_valexp_pairs.end(),
          std::back_inserter(field_names),
          [](const FieldValueExpPair &p) { return std::string{p.field.field_name()}; });

      // 寻找对应存在当前字段的索引
      index = table->find_index_by_field(field_names);  // 不考虑最左匹配, 字段全部匹配就走索引
                                                        // TODO(oldcb): 最左匹配
      if (nullptr != index) {
        break;
      }
    }
  }

  // 重要! 注意删除不能走索引扫描
  // 因为如果删超过一半节点的相同record, 会导致B+树节点合并
  if (table_get_oper.readonly() && index != nullptr) {
    ASSERT(value_expr != nullptr, "got an index but value expr is null ?");

    // 过滤条件中的值
    std::vector<Value> values;
    values.reserve(field_valexp_pairs.size());
    std::transform(field_valexp_pairs.begin(),
        field_valexp_pairs.end(),
        std::back_inserter(values),
        [](const FieldValueExpPair &v) { return v.value->get_value(); });

    // 过滤条件中的字段
    std::vector<FieldMeta> value_metas;
    values.reserve(field_valexp_pairs.size());
    std::transform(field_valexp_pairs.begin(),
        field_valexp_pairs.end(),
        std::back_inserter(value_metas),
        [](const FieldValueExpPair &v) { return *v.field.meta(); });

    // 走联合索引
    IndexScanPhysicalOperator *index_scan_oper = new IndexScanPhysicalOperator(table,
        index,
        table_get_oper.readonly(),
        values,
        true /*left_inclusive*/,
        values,
        true /*right_inclusive*/,
        value_metas);

    index_scan_oper->set_predicates(std::move(predicates));
    oper = unique_ptr<PhysicalOperator>(index_scan_oper);
    LOG_TRACE("use index scan");
  } else {
    auto table_scan_oper = new TableScanPhysicalOperator(table, table_get_oper.readonly());
    table_scan_oper->set_predicates(std::move(predicates));
    oper = unique_ptr<PhysicalOperator>(table_scan_oper);
    LOG_TRACE("use table scan");
  }

  return RC::SUCCESS;
}

// ORDERBY 下面必须挂对应的算子
RC PhysicalPlanGenerator::create_plan(OrderByLogicalOperator &orderby_oper, std::unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = orderby_oper.children();

  RC rc = RC::SUCCESS;
  unique_ptr<PhysicalOperator> child_phy_oper;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, child_phy_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create project logical operator's child physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }
  auto ptr = new OrderbyPhysicalOperator(std::move(orderby_oper.field_expressions), std::move(orderby_oper.sortTypes));
  std::unique_ptr<PhysicalOperator> orderby_phy_oper(ptr);
  orderby_phy_oper->add_child(std::move(child_phy_oper));
  oper.swap(orderby_phy_oper);
  return rc;
}

// aggr op 下面必须挂一个全表扫描的算子
RC PhysicalPlanGenerator::create_plan(AggregationLogicalOperator &aggr_oper, unique_ptr<PhysicalOperator> &oper)
{

  vector<unique_ptr<LogicalOperator>> &child_opers = aggr_oper.children();
  assert(child_opers.size() == 1);

  RC rc = RC::SUCCESS;
  unique_ptr<PhysicalOperator> table_scan_phy_oper;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, table_scan_phy_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create project logical operator's child physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }
  unique_ptr<PhysicalOperator> aggregation_oper(new AggregationPhysicalOperator(std::move(aggr_oper.expressions())));
  aggregation_oper->add_child(std::move(table_scan_phy_oper));

  oper.swap(aggregation_oper);
  return rc;
}

// groupby 算子，应该在predict、全表扫描上面、或者join上面
RC PhysicalPlanGenerator::create_plan(GroupbyLogicalOperator &groupby_oper, unique_ptr<PhysicalOperator> &oper)
{

  vector<unique_ptr<LogicalOperator>> &child_opers = groupby_oper.children();
  assert(child_opers.size() == 1);

  RC rc = RC::SUCCESS;
  unique_ptr<PhysicalOperator> child_phy_oper;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, child_phy_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create project logical operator's child physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  unique_ptr<PhysicalOperator> groupby_phyoper(new GroupbyPhysicalOperator(
      std::move(groupby_oper.projects), groupby_oper.having, std::move(groupby_oper.groupbys)));
  groupby_phyoper->add_child(std::move(child_phy_oper));

  oper.swap(groupby_phyoper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(PredicateLogicalOperator &pred_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &children_opers = pred_oper.children();
  ASSERT(children_opers.size() == 1, "predicate logical operator's sub oper number should be 1");

  LogicalOperator &child_oper = *children_opers.front();

  unique_ptr<PhysicalOperator> child_phy_oper;
  RC rc = create(child_oper, child_phy_oper);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to create child operator of predicate operator. rc=%s", strrc(rc));
    return rc;
  }

  vector<unique_ptr<Expression>> &expressions = pred_oper.expressions();
  ASSERT(expressions.size() == 1, "predicate logical operator's children should be 1");

  // 这是一个conjunction的表达式，由多个cmp expr组成，需要把里面和子查询有关的，创建成物理算子
  if (expressions[0]->type() == ExprType::CONJUNCTION) {
    auto conj_expr = static_cast<ConjunctionExpr *>(expressions[0].get());
    for (auto &expression : conj_expr->children()) {
      if (expression->type() == ExprType::COMPARISON) {
        ComparisonExpr *cmp_expr = static_cast<ComparisonExpr *>(expression.get());
        if (cmp_expr->left()->type() == ExprType::SUBQUERY) {
          auto sub_query_expr = static_cast<SubQueryExpr *>(cmp_expr->left().get());
          rc = create(*sub_query_expr->oper, sub_query_expr->phy_oper);
          if (rc != RC::SUCCESS) {
            return rc;
          }
        }
        if (cmp_expr->right()->type() == ExprType::SUBQUERY) {
          auto sub_query_expr = static_cast<SubQueryExpr *>(cmp_expr->right().get());
          rc = create(*sub_query_expr->oper, sub_query_expr->phy_oper);
          if (rc != RC::SUCCESS) {
            return rc;
          }
        }
      }
    }
  } else if (expressions[0]->type() == ExprType::COMPARISON) {
    ComparisonExpr *cmp_expr = static_cast<ComparisonExpr *>(expressions[0].get());
    if (cmp_expr->left()->type() == ExprType::SUBQUERY) {
      auto sub_query_expr = static_cast<SubQueryExpr *>(cmp_expr->left().get());
      rc = create(*sub_query_expr->oper, sub_query_expr->phy_oper);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
    if (cmp_expr->right()->type() == ExprType::SUBQUERY) {
      auto sub_query_expr = static_cast<SubQueryExpr *>(cmp_expr->right().get());
      rc = create(*sub_query_expr->oper, sub_query_expr->phy_oper);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
  }

  unique_ptr<Expression> expression = std::move(expressions.front());
  oper = unique_ptr<PhysicalOperator>(new PredicatePhysicalOperator(std::move(expression)));
  oper->add_child(std::move(child_phy_oper));
  return rc;
}

RC PhysicalPlanGenerator::create_plan(ProjectLogicalOperator &project_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = project_oper.children();

  unique_ptr<PhysicalOperator> child_phy_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, child_phy_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create project logical operator's child physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  ProjectPhysicalOperator *project_operator = new ProjectPhysicalOperator(std::move(project_oper.projects));

  if (child_phy_oper) {
    project_operator->add_child(std::move(child_phy_oper));
  }
  oper = unique_ptr<PhysicalOperator>(project_operator);
  LOG_TRACE("create a project physical operator");
  return rc;
}

RC PhysicalPlanGenerator::create_plan(InsertLogicalOperator &insert_oper, unique_ptr<PhysicalOperator> &oper)
{
  Table *table = insert_oper.table();
  vector<Value> &values = insert_oper.values();
  InsertPhysicalOperator *insert_phy_oper = new InsertPhysicalOperator(table, std::move(values));
  oper.reset(insert_phy_oper);
  return RC::SUCCESS;
}

RC PhysicalPlanGenerator::create_plan(DeleteLogicalOperator &delete_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = delete_oper.children();

  unique_ptr<PhysicalOperator> child_physical_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, child_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  oper = unique_ptr<PhysicalOperator>(new DeletePhysicalOperator(delete_oper.table()));

  if (child_physical_oper) {
    oper->add_child(std::move(child_physical_oper));
  }
  return rc;
}

RC PhysicalPlanGenerator::create_plan(UpdateLogicalOperator &update_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = update_oper.children();

  unique_ptr<PhysicalOperator> child_physical_oper;

  RC rc = RC::SUCCESS;
  if (!child_opers.empty()) {
    LogicalOperator *child_oper = child_opers.front().get();
    rc = create(*child_oper, child_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical operator. rc=%s", strrc(rc));
      return rc;
    }
  }

  for (const auto &expr : update_oper.value_exprs) {
    if (expr->type() == ExprType::SUBQUERY) {
      auto sub_query = static_cast<SubQueryExpr *>(expr.get());
      rc = create(*sub_query->oper, sub_query->phy_oper);
      if (rc != RC::SUCCESS) {
        return rc;
      }
    }
  }

  oper = unique_ptr<PhysicalOperator>(
      new UpdatePhysicalOperator(update_oper.table(), std::move(update_oper.value_exprs), update_oper.field_metas()));

  if (child_physical_oper) {
    oper->add_child(std::move(child_physical_oper));
  }
  return rc;
}

RC PhysicalPlanGenerator::create_plan(ExplainLogicalOperator &explain_oper, unique_ptr<PhysicalOperator> &oper)
{
  vector<unique_ptr<LogicalOperator>> &child_opers = explain_oper.children();

  RC rc = RC::SUCCESS;
  unique_ptr<PhysicalOperator> explain_physical_oper(new ExplainPhysicalOperator);
  for (unique_ptr<LogicalOperator> &child_oper : child_opers) {
    unique_ptr<PhysicalOperator> child_physical_oper;
    rc = create(*child_oper, child_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create child physical operator. rc=%s", strrc(rc));
      return rc;
    }

    explain_physical_oper->add_child(std::move(child_physical_oper));
  }

  oper = std::move(explain_physical_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(JoinLogicalOperator &join_oper, unique_ptr<PhysicalOperator> &oper)
{
  RC rc = RC::SUCCESS;

  vector<unique_ptr<LogicalOperator>> &child_opers = join_oper.children();
  if (child_opers.size() != 2) {
    return RC::SUCCESS;
  }

  unique_ptr<PhysicalOperator> join_physical_oper;

  // 如果有跨表，且等值表达式的话。那么使用hash, join
  if (join_oper.has_equal_cmp_expression()) {
    auto hash_join = new HashJoinPhysicalOperator;
    hash_join->set_expressions(std::move(join_oper.expressions()));
    join_physical_oper.reset(static_cast<PhysicalOperator *>(hash_join));
  } else {
    auto nest_loop = new NestedLoopJoinPhysicalOperator;
    nest_loop->set_expressions(std::move(join_oper.expressions()));
    join_physical_oper.reset(static_cast<PhysicalOperator *>(nest_loop));
  }

  for (auto &child_oper : child_opers) {
    unique_ptr<PhysicalOperator> child_physical_oper;
    rc = create(*child_oper, child_physical_oper);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to create physical child oper. rc=%s", strrc(rc));
      return rc;
    }

    join_physical_oper->add_child(std::move(child_physical_oper));
  }

  oper = std::move(join_physical_oper);
  return rc;
}

RC PhysicalPlanGenerator::create_plan(CalcLogicalOperator &logical_oper, std::unique_ptr<PhysicalOperator> &oper)
{
  RC rc = RC::SUCCESS;
  CalcPhysicalOperator *calc_oper = new CalcPhysicalOperator(std::move(logical_oper.expressions()));
  oper.reset(calc_oper);
  return rc;
}
