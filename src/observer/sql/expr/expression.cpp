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
// Created by Wangyunlai on 2022/07/05.
//
#include "common/rc.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/db/db.h"
#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/physical_operator.h"

#include "sql/stmt/select_stmt.h"
#include "storage/record/record.h"

using namespace std;

RC Expression::create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
    const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp, Db *db)
{
  if (expr->type == PExpType::UNARY) {
    auto uexpr = expr->uexp;
    if (uexpr->is_attr) {
      return FieldExpr::create_expression(expr, parent_table_map, cur_table_map, res_expr, comp, db);
    } else {
      return ValueExpr::create_expression(expr, parent_table_map, cur_table_map, res_expr, comp, db);
    }
  } else if (expr->type == PExpType::ARITHMETIC) {
    return ArithmeticExpr::create_expression(expr, parent_table_map, cur_table_map, res_expr, comp, db);
  } else if (expr->type == PExpType::LIST) {
    return ListExpr::create_expression(expr, parent_table_map, cur_table_map, res_expr, comp, db);
  } else if (expr->type == PExpType::SUBQUERY) {
    return SubQueryExpr::create_expression(expr, parent_table_map, cur_table_map, res_expr, comp, db);
  } else if (expr->type == PExpType::AGGRFUNC) {
    return AggretationExpr::create_expression(expr, parent_table_map, cur_table_map, res_expr, comp, db);
  }
  return RC::UNIMPLENMENT;
}

/////////////////////////////////////////////////////////////////////////////////
RC FieldExpr::get_value(const Tuple &tuple, Value &value) const
{
  return tuple.find_cell(TupleCellSpec(table_name(), field_name()), value);
}
RC FieldExpr::get_record(const Tuple &tuple, RecordPos &rid) const
{
  return tuple.find_record(TupleCellSpec(table_name(), field_name()), rid);
}

void FieldExpr::get_fieldexprs(Expression *expr, std::vector<Expression *> &field_exprs)
{
  if (expr == nullptr) {
    return;
  }
  switch (expr->type()) {
    case ExprType::FIELD: {
      field_exprs.emplace_back(expr);
      break;
    }
    case ExprType::LIST: {  // 处理(max(col) + 1)的情况
      auto lexp = static_cast<ListExpr *>(expr);
      get_fieldexprs(lexp->expr_at(0), field_exprs);
      break;
    }
    case ExprType::ARITHMETIC: {
      auto aexp = static_cast<ArithmeticExpr *>(expr);
      get_fieldexprs(aexp->left().get(), field_exprs);
      get_fieldexprs(aexp->right().get(), field_exprs);
      break;
    }
    default: break;
  }
  return;
}

RC FieldExpr::create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
    const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp, Db *db)
{

  assert(PExpType::UNARY == expr->type);
  auto uexpr = expr->uexp;
  assert(uexpr->is_attr);
  const auto &attr = uexpr->attr;
  const char *table_name = attr.relation_name.c_str();
  const char *field_name = attr.attribute_name.c_str();

  if (cur_table_map.empty()) {
    LOG_WARN("invalid. empty, table");
    return RC::SCHEMA_FIELD_MISSING;
  }

  Table *table = nullptr;
  if (common::is_blank(table_name)) {
    if (cur_table_map.size() != 1) {
      LOG_WARN("invalid. I do not know the attr's table. attr=%s", field_name);
      return RC::SCHEMA_FIELD_MISSING;
    }
    table = cur_table_map.begin()->second;
  } else {
    auto iter = cur_table_map.find(table_name);
    if (iter != cur_table_map.end()) {
      table = iter->second;
    }
    if (table == nullptr) {
      auto iter2 = parent_table_map.find(table_name);
      if (iter2 != cur_table_map.end()) {
        table = iter2->second;
      }
    }
  }

  if (table == nullptr) {
    table = db->find_table(table_name);
  }
  if (nullptr == table) {
    LOG_WARN("No such table: attr.relation_name: %s", attr.relation_name.c_str());
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  const FieldMeta *field_meta = table->table_meta().field(attr.attribute_name.c_str());
  if (nullptr == field_meta) {
    LOG_WARN("no such field in table: table %s, field %s", table->name(), attr.attribute_name.c_str());
    table = nullptr;
    return RC::SCHEMA_FIELD_NOT_EXIST;
  }

  // // 支持视图, 如果table是视图, 要恢复对应的字段表名
  // const Table *query_table = nullptr;
  // if (table->is_view()) {
  //   query_table = table->table_meta().view_table(attr.attribute_name.c_str());
  // } else {
  //   query_table = table;
  // }
  res_expr = new FieldExpr(table, field_meta);
  return RC::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
RC ValueExpr::get_value(const Tuple &tuple, Value &value) const
{
  value = value_;
  return RC::SUCCESS;
}

RC ValueExpr::get_record(const Tuple &tuple, RecordPos &rid) const { return RC::NOTFOUND; }

RC ValueExpr::create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
    const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp, Db *db)
{
  assert(PExpType::UNARY == expr->type);
  auto uexpr = expr->uexp;
  assert(uexpr->is_attr == 0);
  res_expr = new ValueExpr(uexpr->value);
  return RC::SUCCESS;
}

/////////////////////////////////////////////////////////////////////////////////
CastExpr::CastExpr(unique_ptr<Expression> child, AttrType cast_type) : child_(std::move(child)), cast_type_(cast_type)
{}

CastExpr::~CastExpr() {}

RC CastExpr::cast(const Value &value, Value &cast_value) const
{
  RC rc = RC::SUCCESS;
  if (this->value_type() == value.attr_type()) {
    cast_value = value;
    return rc;
  }

  switch (cast_type_) {
    case BOOLEANS: {
      bool val = value.get_boolean();
      cast_value.set_boolean(val);
    } break;
    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported convert from type %d to %d", child_->value_type(), cast_type_);
    }
  }
  return rc;
}

RC CastExpr::get_value(const Tuple &tuple, Value &cell) const
{
  RC rc = child_->get_value(tuple, cell);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(cell, cell);
}

RC CastExpr::get_record(const Tuple &tuple, RecordPos &rid) const
{
  RC rc = child_->get_record(tuple, rid);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  return RC::SUCCESS;
}

RC CastExpr::try_get_value(Value &value) const
{
  RC rc = child_->try_get_value(value);
  if (rc != RC::SUCCESS) {
    return rc;
  }

  return cast(value, value);
}

////////////////////////////////////////////////////////////////////////////////

ComparisonExpr::ComparisonExpr(CompOp comp, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : comp_(comp), left_(std::move(left)), right_(std::move(right))
{}

ComparisonExpr::~ComparisonExpr() {}

bool ComparisonExpr::is_like(const std::string &s, const std::string &p) const
{
  int n = s.size(), m = p.size();
  // 动态规划
  vector<vector<bool>> f(n + 1, vector<bool>(m + 1));
  f[0][0] = true;
  for (int i = 1; i <= m; i++) {
    if (p[i - 1] != '%')
      break;
    f[0][i] = true;
  }
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j <= m; j++) {
      if (p[j - 1] == '%') {
        int k = j - 1;
        while (k >= 0 && p[k] == '%')
          k--;
        if (k < 0) {
          f[i][j] = true;
        } else {
          for (int u = 0; u < i; u++) {
            if (s[u] == p[k] || p[k] == '_') {
              f[i][j] = f[i][j] | f[u][k];
            }
          }
        }
      } else if (p[j - 1] == '_') {
        f[i][j] = f[i - 1][j - 1];
      } else {
        f[i][j] = (s[i - 1] == p[j - 1] && f[i - 1][j - 1]);
      }
    }
  }
  return f[n][m];
}

RC ComparisonExpr::compare_value(const Value &left, const Value &right, bool &result) const
{
  RC rc = RC::SUCCESS;
  // LIKE
  if (comp_ == CompOp::LIKE || comp_ == CompOp::NOT_LIKE) {
    if (left.attr_type() != AttrType::CHARS || right.attr_type() != AttrType::CHARS) {
      result = false;
    }
    result = is_like(left.get_string(), right.get_string());
    if (comp_ == CompOp::NOT_LIKE)
      result = !result;
    return rc;
  }

  // NULL
  if (comp_ == CompOp::IS_NULL) {
    if (left.attr_type() == AttrType::NULLS) {
      result = true;
    } else {
      result = false;
    }
    return rc;
  } else if (comp_ == CompOp::IS_NOT_NULL) {
    if (left.attr_type() != AttrType::NULLS) {
      result = true;
    } else {
      result = false;
    }
    return rc;
  } else {
    if (left.attr_type() == AttrType::NULLS || right.attr_type() == AttrType::NULLS) {
      result = false;
      return rc;
    }
  }

  // Compare
  int cmp_result = left.compare(right);
  result = false;
  switch (comp_) {
    case CompOp::EQUAL_TO: {
      result = (0 == cmp_result);
    } break;
    case CompOp::LESS_EQUAL: {
      result = (cmp_result <= 0);
    } break;
    case CompOp::NOT_EQUAL: {
      result = (cmp_result != 0);
    } break;
    case CompOp::LESS_THAN: {
      result = (cmp_result < 0);
    } break;
    case CompOp::GREAT_EQUAL: {
      result = (cmp_result >= 0);
    } break;
    case CompOp::GREAT_THAN: {
      result = (cmp_result > 0);
    } break;
    default: {
      LOG_WARN("unsupported comparison. %d", comp_);
      rc = RC::INTERNAL;
    } break;
  }

  return rc;
}

RC ComparisonExpr::try_get_value(Value &cell) const
{
  if (left_->type() == ExprType::VALUE && right_->type() == ExprType::VALUE) {
    ValueExpr *left_value_expr = static_cast<ValueExpr *>(left_.get());
    ValueExpr *right_value_expr = static_cast<ValueExpr *>(right_.get());
    const Value &left_cell = left_value_expr->get_value();
    const Value &right_cell = right_value_expr->get_value();

    bool value = false;
    RC rc = compare_value(left_cell, right_cell, value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to compare tuple cells. rc=%s", strrc(rc));
    } else {
      cell.set_boolean(value);
    }
    return rc;
  }

  return RC::INVALID_ARGUMENT;
}

RC ComparisonExpr::get_one_row_value(const std::unique_ptr<Expression> &expr, const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  if (expr->type() == ExprType::FIELD || expr->type() == ExprType::VALUE) {
    rc = expr->get_value(tuple, value);
  } else if (expr->type() == ExprType::SUBQUERY) {
    auto sub_query = static_cast<SubQueryExpr *>(expr.get());
    // rc = sub_query->get_one_row_value(tuple, value);
    // 用于复杂子查询
    rc = sub_query->get_and_set_one_row_value(tuple, value, parent_tuple_.get());
  } else {
    LOG_WARN("不支持的表达式类型 %s", __LINE__);
  }
  return rc;
}

RC ComparisonExpr::get_value(const Tuple &tuple, Value &value) const
{
  Value left_value;
  Value right_value;
  RC rc = RC::SUCCESS;

  switch (comp_) {
    // 右表达式只支持子查询，以及List, in [not in]
    case CompOp::NOT_IN:
    case CompOp::IN: {
      // 左边只能有一个value
      bool is_in = false;
      rc = get_one_row_value(left_, tuple, left_value);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      if (right_->type() == ExprType::SUBQUERY) {
        auto sub_query = static_cast<SubQueryExpr *>(right_.get());
        sub_query->phy_oper->open(nullptr);
        while ((rc = sub_query->get_and_set_value(tuple, right_value, parent_tuple_.get())) == RC::SUCCESS) {
          if (left_value.compare(right_value) == 0) {
            is_in = true;
            break;
          }
        }
      } else if (right_->type() == ExprType::LIST) {
        static_cast<ListExpr *>(right_.get())->reset();
        while ((rc = right_->get_value(tuple, right_value)) == RC::SUCCESS) {
          if (left_value.compare(right_value) == 0) {
            is_in = true;
            break;
          }
        }
      } else {
        return RC::SQL_SYNTAX;
      }

      if (comp_ == CompOp::NOT_IN)
        is_in = !is_in;
      if (rc == RC::RECORD_EOF) {
        rc = RC::SUCCESS;
      }
      if (rc != RC::SUCCESS) {
        LOG_WARN("子表达式出现了非EOF的返回结果");
        return rc;
      }
      value.set_boolean(is_in);
    } break;
    case CompOp::NOT_EXISTS:
    case CompOp::EXISTS: {
      if (right_->type() != ExprType::SUBQUERY) {
        return RC::SQL_SYNTAX;
      }
      auto sub_query = static_cast<SubQueryExpr *>(right_.get());
      sub_query->phy_oper->open(nullptr);
      RC rc = sub_query->phy_oper->open(nullptr);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      rc = sub_query->phy_oper->next();
      if (rc == RC::SUCCESS) {
        value.set_boolean(comp_ == CompOp::EXISTS ? true : false);
        return rc;
      } else if (rc == RC::RECORD_EOF) {
        value.set_boolean(comp_ == CompOp::NOT_EXISTS ? true : false);
        return rc;
      }
    } break;

    default: {
      // 正常就是原来的运算符，只不过左右表达式是子查询的时候，需要求一元表达式
      SubQueryExpr *sub_query = nullptr;

      // 获取左表达式的值
      if (left_->type() == ExprType::SUBQUERY) {
        sub_query = static_cast<SubQueryExpr *>(left_.get());
        // rc = sub_query->get_one_row_value(tuple, left_value);
        // 用于复杂子查询
        rc = sub_query->get_and_set_one_row_value(tuple, left_value, parent_tuple_.get());
      } else {
        rc = left_->get_value(tuple, left_value);
        if (rc != RC::SUCCESS && parent_tuple_ != nullptr) {
          rc = left_->get_value(*parent_tuple_, left_value);
        }
      }
      if (rc != RC::SUCCESS) {
        LOG_WARN("左表达式非法");
        return rc;
      }
      // 获取右表达式的值
      if (right_->type() == ExprType::SUBQUERY) {
        sub_query = static_cast<SubQueryExpr *>(right_.get());
        // rc = sub_query->get_one_row_value(tuple, right_value);
        // 用于复杂子查询
        rc = sub_query->get_and_set_one_row_value(tuple, right_value, parent_tuple_.get());
      } else {
        rc = right_->get_value(tuple, right_value);
        if (rc != RC::SUCCESS && parent_tuple_ != nullptr) {
          rc = right_->get_value(*parent_tuple_, right_value);
        }
      }
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to get_one_row. rc=%d:%s", rc, strrc(rc));
        return rc;
      }

      bool res;
      rc = compare_value(left_value, right_value, res);
      value.set_boolean(res);
    } break;
  }
  return rc;
}

RC ComparisonExpr::get_record(const Tuple &tuple, RecordPos &rid) const { return RC::NOTFOUND; }

// 反转比较符号
void ComparisonExpr::reverse_comp()
{
  switch (comp_) {
    case CompOp::GREAT_EQUAL: comp_ = CompOp::LESS_EQUAL; break;
    case CompOp::GREAT_THAN: comp_ = CompOp::LESS_THAN; break;
    case CompOp::LESS_EQUAL: comp_ = CompOp::GREAT_EQUAL; break;
    case CompOp::LESS_THAN: comp_ = CompOp::GREAT_THAN; break;
    default: break;
  }
}

////////////////////////////////////////////////////////////////////////////////
ConjunctionExpr::ConjunctionExpr(Type type, vector<unique_ptr<Expression>> &children)
    : conjunction_type_(type), children_(std::move(children))
{}

RC ConjunctionExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  if (children_.empty()) {
    value.set_boolean(true);
    return rc;
  }

  Value tmp_value;
  for (const unique_ptr<Expression> &expr : children_) {
    rc = expr->get_value(tuple, tmp_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value by child expression. rc=%s", strrc(rc));
      return rc;
    }
    bool bool_value = tmp_value.get_boolean();
    if ((conjunction_type_ == Type::AND && !bool_value) || (conjunction_type_ == Type::OR && bool_value)) {
      value.set_boolean(bool_value);
      return rc;
    }
  }

  bool default_value = (conjunction_type_ == Type::AND);
  value.set_boolean(default_value);
  return rc;
}

RC ConjunctionExpr::get_record(const Tuple &tuple, RecordPos &rid) const { return RC::NOTFOUND; }

////////////////////////////////////////////////////////////////////////////////

ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, Expression *left, Expression *right)
    : arithmetic_type_(type), left_(left), right_(right)
{}
ArithmeticExpr::ArithmeticExpr(ArithmeticExpr::Type type, unique_ptr<Expression> left, unique_ptr<Expression> right)
    : arithmetic_type_(type), left_(std::move(left)), right_(std::move(right))
{}

AttrType ArithmeticExpr::value_type() const
{
  if (!right_) {
    return left_->value_type();
  }

  if (left_->value_type() == AttrType::INTS && right_->value_type() == AttrType::INTS &&
      arithmetic_type_ != Type::DIV) {
    return AttrType::INTS;
  }

  return AttrType::FLOATS;
}

RC ArithmeticExpr::calc_value(const Value &left_value, const Value &right_value, Value &value) const
{
  RC rc = RC::SUCCESS;

  if (left_value.is_null() || right_value.is_null()) {
    value.set_null();
    return rc;
  }

  const AttrType target_type = value_type();

  switch (arithmetic_type_) {
    case Type::ADD: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() + right_value.get_int());
      } else {
        value.set_float(left_value.get_float() + right_value.get_float());
      }
    } break;

    case Type::SUB: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() - right_value.get_int());
      } else {
        value.set_float(left_value.get_float() - right_value.get_float());
      }
    } break;

    case Type::MUL: {
      if (target_type == AttrType::INTS) {
        value.set_int(left_value.get_int() * right_value.get_int());
      } else {
        value.set_float(left_value.get_float() * right_value.get_float());
      }
    } break;

    case Type::DIV: {
      if (target_type == AttrType::INTS) {
        if (right_value.get_int() == 0) {
          // NOTE:
          // 设置为整数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为整数最大值。
          // value.set_int(numeric_limits<int>::max());
          value.set_null();
        } else {
          value.set_int(left_value.get_int() / right_value.get_int());
        }
      } else {
        if (right_value.get_float() > -EPSILON && right_value.get_float() < EPSILON) {
          // NOTE:
          // 设置为浮点数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为浮点数最大值。
          // value.set_int(numeric_limits<int>::max());
          value.set_null();
        } else {
          value.set_float(left_value.get_float() / right_value.get_float());
        }
      }
    } break;

    case Type::NEGATIVE: {
      if (target_type == AttrType::INTS) {
        value.set_int(-left_value.get_int());
      } else {
        value.set_float(-left_value.get_float());
      }
    } break;

    default: {
      rc = RC::INTERNAL;
      LOG_WARN("unsupported arithmetic type. %d", arithmetic_type_);
    } break;
  }
  return rc;
}

RC ArithmeticExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->get_value(tuple, left_value);
  if (left_->type() == ExprType::LIST) {
    auto list_expr = static_cast<ListExpr *>(left_.get());
    list_expr->reset();
  }
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  if (right_) {
    rc = right_->get_value(tuple, right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
    if (right_->type() == ExprType::LIST) {
      auto list_expr = static_cast<ListExpr *>(right_.get());
      list_expr->reset();
    }
    return calc_value(left_value, right_value, value);
  } else {
    // 只有负数是这种情况
    if (left_value.attr_type() == AttrType::INTS) {
      value.set_int(-left_value.get_int());
    } else if (left_value.attr_type() == AttrType::FLOATS) {
      value.set_float(-left_value.get_float());
    } else if (left_value.attr_type() == AttrType::NULLS) {
      value.set_null();
    } else {
      LOG_ERROR("calc error: %s", strrc(RC::NOT_IMPLEMENT));
      return RC::NOT_IMPLEMENT;
    }
  }
  return rc;
}

RC ArithmeticExpr::try_get_value(Value &value) const
{
  RC rc = RC::SUCCESS;

  Value left_value;
  Value right_value;

  rc = left_->try_get_value(left_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }

  if (right_) {
    rc = right_->try_get_value(right_value);
    if (rc != RC::SUCCESS) {
      LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
      return rc;
    }
  }

  return calc_value(left_value, right_value, value);
}

RC ArithmeticExpr::get_record(const Tuple &tuple, RecordPos &rid) const { return RC::NOTFOUND; }

RC ArithmeticExpr::create_expression(const PExpr *expr,
    const std::unordered_map<std::string, Table *> &parent_table_map,
    const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp, Db *db)
{
  assert(PExpType::ARITHMETIC == expr->type);
  Expression *left_expr = nullptr;
  Expression *right_expr = nullptr;
  RC rc = RC::SUCCESS;
  if (expr->aexp->left != nullptr) {
    rc = Expression::create_expression(expr->aexp->left, parent_table_map, cur_table_map, left_expr);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("ArithmeticExpr Create Left Expression Failed. RC = %d:%s", rc, strrc(rc));
      return rc;
    }
  }
  if (expr->aexp->right) {
    rc = Expression::create_expression(expr->aexp->right, parent_table_map, cur_table_map, right_expr);
    if (rc != RC::SUCCESS) {
      LOG_ERROR("ArithmeticExpr Create Right Expression Failed. RC = %d:%s", rc, strrc(rc));
      delete left_expr;
      return rc;
    }
  }
  Type type;
  if (expr->aexp->type == PArithmeticType::ADD) {
    type = Type::ADD;
  } else if (expr->aexp->type == PArithmeticType::SUB) {
    type = Type::SUB;
  } else if (expr->aexp->type == PArithmeticType::MUL) {
    type = Type::MUL;
  } else if (expr->aexp->type == PArithmeticType::DIV) {
    type = Type::DIV;
  } else if (expr->aexp->type == PArithmeticType::NEGATIVE) {
    type = Type::NEGATIVE;
  } else {
    assert(0);
  }
  auto a_Expr = new ArithmeticExpr(type, left_expr, right_expr);
  res_expr = a_Expr;
  return RC::SUCCESS;
}

////////////////////////////////////////////////////////////////////////////////
RC AggretationExpr::get_value(const Tuple &tuple, Value &value) const
{
  if (has_value) {
    value = value_;
    return RC::SUCCESS;
  }
  RC rc = RC::SUCCESS;
  switch (aggr_func_type_) {
    case AggrFuncType::MIN:
    case AggrFuncType::MAX:
    case AggrFuncType::SUM:
    case AggrFuncType::AVG: rc = field_expr_->get_value(tuple, value); break;
    case AggrFuncType::COUNT:
      rc = field_expr_->get_value(tuple, value);
      if (!value.is_null()) {
        value = Value(1);
      }
      break;
    case AggrFuncType::COUNT_STAR: value.set_int(1); break;
    default: break;
  }
  return rc;
}

RC AggretationExpr::get_record(const Tuple &tuple, RecordPos &rid) const { return RC::NOTFOUND; }

void AggretationExpr::get_aggrfuncexprs(Expression *expr, std::vector<Expression *> &aggrfunc_exprs)
{
  if (expr == nullptr) {
    return;
  }
  switch (expr->type()) {
    case ExprType::AGGRFUNCTION: {
      aggrfunc_exprs.emplace_back(expr);
      break;
    }
    case ExprType::LIST: {  // 处理(max(col) + 1)的情况
      auto lexp = static_cast<ListExpr *>(expr);
      get_aggrfuncexprs(lexp->expr_at(0), aggrfunc_exprs);
      break;
    }
    case ExprType::ARITHMETIC: {
      auto bexp = static_cast<ArithmeticExpr *>(expr);
      get_aggrfuncexprs(bexp->left().get(), aggrfunc_exprs);
      get_aggrfuncexprs(bexp->right().get(), aggrfunc_exprs);
      break;
    }
    default: break;
  }
}

RC AggretationExpr::create_expression(const PExpr *expr,
    const std::unordered_map<std::string, Table *> &parent_table_map,
    const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp, Db *db)
{
  assert(expr->agexp->expr);
  if (expr->agexp->is_star) {
    AggretationExpr *agexpr = new AggretationExpr(nullptr, expr->agexp->type, expr->agexp->is_star);
    res_expr = agexpr;
    return RC::SUCCESS;
  } else {
    Expression *sub_expr = nullptr;
    RC rc = Expression::create_expression(expr->agexp->expr, parent_table_map, cur_table_map, sub_expr, comp, db);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    AggretationExpr *agexpr = new AggretationExpr(sub_expr, expr->agexp->type, expr->agexp->is_star);
    res_expr = agexpr;
    return rc;
  }
}

////////////////////////////////////////////////////////////////////////////////

SubQueryExpr::SubQueryExpr() {}

SubQueryExpr::~SubQueryExpr()
{
  if (subquery_stmt != nullptr) {
    delete subquery_stmt;
  }
}

RC SubQueryExpr::get_and_set_value(const Tuple &tuple, Value &value, Tuple *parent) const
{
  if (phy_oper == nullptr) {
    return RC::RECORD_EOF;
  }
  //  ------ 将当前的tuple和父查询传来tuple需要组合后，再传到下面的子查询 --------
  std::shared_ptr<Tuple> join_tuple;
  Tuple *right_tuple = nullptr;

  if (parent == nullptr) {
    join_tuple.reset(tuple.copy_tuple());
  } else {
    right_tuple = tuple.copy_tuple();
    auto tmp = std::make_shared<JoinedTuple>();
    tmp->set_left(parent);
    tmp->set_right(right_tuple);
    join_tuple = tmp;
  }
  phy_oper->set_sub_query(join_tuple);
  if (right_tuple != nullptr) {
    delete right_tuple;
  }
  //  ------ 将当前的tuple和父查询传来tuple需要组合后，再传到下面的子查询 --------

  RC rc = RC::SUCCESS;
  rc = phy_oper->next();
  if (rc != RC::SUCCESS) {
    return rc;
  }
  Tuple *sub_tuple = phy_oper->current_tuple();
  // 不支持多列比较的情况
  if (sub_tuple->cell_num() != 1) {
    LOG_ERROR("sub query return multi cols");
    return RC::SQL_SYNTAX;
  }
  rc = sub_tuple->cell_at(0, value);
  return rc;
}

RC SubQueryExpr::get_value(const Tuple &tuple, Value &value) const
{
  if (phy_oper == nullptr) {
    return RC::RECORD_EOF;
  }
  RC rc = RC::SUCCESS;
  rc = phy_oper->next();
  if (rc != RC::SUCCESS) {
    return rc;
  }
  Tuple *sub_tuple = phy_oper->current_tuple();
  // 不支持多列比较的情况
  if (sub_tuple->cell_num() != 1) {
    LOG_ERROR("sub query return multi cols");
    return RC::SQL_SYNTAX;
  }
  rc = sub_tuple->cell_at(0, value);
  return rc;
}

RC SubQueryExpr::get_record(const Tuple &tuple, RecordPos &rid) const { return RC::NOTFOUND; }

/**
 * @description: 如果返回的数据行数<=1, 则返回RC::SUCCESS
 * @param {Tuple} &tuple
 * @param {Value} &value
 * @return {*}
 */
RC SubQueryExpr::get_one_row_value(const Tuple &tuple, Value &value)
{
  RC rc = RC::SUCCESS;
  rc = phy_oper->open(nullptr);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  // 里面会调用next方法
  rc = get_value(tuple, value);
  // 空值也返回成功, 但是返回null
  if (rc == RC::RECORD_EOF) {
    value.set_null();
    return RC::SUCCESS;
  }
  if (phy_oper->current_tuple()->cell_num() > 1) {
    return RC::TOO_MANY_COLS;
  }
  // 否则这里value有值了，但还是调用一次，多于1条记录
  rc = get_value(tuple, value);
  if (rc == RC::RECORD_EOF) {
    return RC::SUCCESS;
  }
  if (rc == RC::SUCCESS) {
    // LOG_WARN("sub query return more than 1 row"); // 对于update, 当且仅当存在修改的tuple且返回多行时才会报该错
    return RC::TOO_MANY_ROW;
  }
  return rc;
}

RC SubQueryExpr::get_and_set_one_row_value(const Tuple &tuple, Value &value, Tuple *parent)
{
  RC rc = RC::SUCCESS;
  //  ------ 将当前的tuple和父查询传来tuple需要组合后，再传到下面的子查询 --------
  std::shared_ptr<Tuple> join_tuple;
  Tuple *right_tuple = nullptr;

  if (parent == nullptr) {
    join_tuple.reset(tuple.copy_tuple());
  } else {
    right_tuple = tuple.copy_tuple();
    auto tmp = std::make_shared<JoinedTuple>();
    tmp->set_left(parent);
    tmp->set_right(right_tuple);
    join_tuple = tmp;
  }
  phy_oper->set_sub_query(join_tuple);
  if (right_tuple != nullptr) {
    delete right_tuple;
  }
  //  ------ 将当前的tuple和父查询传来tuple需要组合后，再传到下面的子查询 --------

  rc = phy_oper->open(nullptr);
  if (rc != RC::SUCCESS && rc != RC::RECORD_EOF) {
    return rc;
  }
  // 里面会调用next方法
  rc = get_value(tuple, value);
  // 空值也返回成功, 但是返回null
  if (rc == RC::RECORD_EOF) {
    value.set_null();
    return RC::SUCCESS;
  }
  if (phy_oper->current_tuple()->cell_num() > 1) {
    return RC::TOO_MANY_COLS;
  }
  // 否则这里value有值了，但还是调用一次，多于1条记录
  rc = get_value(tuple, value);
  if (rc == RC::RECORD_EOF) {
    return RC::SUCCESS;
  }
  if (rc == RC::SUCCESS) {
    LOG_WARN("sub query return more than 1 row");
    return RC::TOO_MANY_ROW;
  }
  return rc;
}

RC SubQueryExpr::create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
    const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp, Db *db)
{
  Stmt *tmp_stmt = nullptr;

  auto new_map = parent_table_map;
  new_map.insert(cur_table_map.begin(), cur_table_map.end());
  RC rc = SelectStmt::create(db, *expr->sexp->sub_select, new_map, tmp_stmt);
  if (rc != RC::SUCCESS) {
    return rc;
  }
  auto subquery_expr = new SubQueryExpr();
  subquery_expr->subquery_stmt = static_cast<SelectStmt *>(tmp_stmt);
  res_expr = subquery_expr;
  return RC::SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////
RC ListExpr::get_value(const Tuple &tuple, Value &value) const
{
  if (idx_ >= values_.size()) {
    return RC::RECORD_EOF;
  }
  RC rc = values_[idx_++]->get_value(tuple, value);
  return rc;
}

RC ListExpr::get_record(const Tuple &tuple, RecordPos &rid) const { return RC::NOTFOUND; }

RC ListExpr::create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
    const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp, Db *db)
{
  assert(expr->lexp != nullptr);
  PListExpr *p_list_expr = expr->lexp;
  ListExpr *list_expr = new ListExpr();
  std::vector<std::unique_ptr<Expression>> vec;
  for (const auto &expr : p_list_expr->expr_list) {
    Expression *tmp;
    RC rc = Expression::create_expression(expr, parent_table_map, cur_table_map, tmp, comp, db);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    vec.emplace_back(tmp);
  }
  list_expr->set_values(std::move(vec));
  res_expr = list_expr;
  return RC::SUCCESS;
}
////////////////////////////////////////////////////////////////////////////////
RC HavingFieldExpr::get_value(const Tuple &tuple, Value &value) const
{
  if (pos_ < 0 && pos_ >= tuple.cell_num()) {
    return RC::INTERNAL;
  }
  return tuple.cell_at(pos_, value);
}

RC HavingFieldExpr::get_record(const Tuple &tuple, RecordPos &rid) const { return RC::NOTFOUND; }
