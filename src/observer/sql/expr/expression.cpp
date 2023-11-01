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

#include "sql/expr/expression.h"
#include "sql/expr/tuple.h"
#include "sql/operator/logical_operator.h"
#include "sql/operator/physical_operator.h"

#include "sql/stmt/select_stmt.h"

using namespace std;

RC FieldExpr::get_value(const Tuple &tuple, Value &value) const
{
  return tuple.find_cell(TupleCellSpec(table_name(), field_name()), value);
}

RC ValueExpr::get_value(const Tuple &tuple, Value &value) const
{
  value = value_;
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


RC ComparisonExpr::get_one_row_value(const std::unique_ptr<Expression>& expr, const Tuple &tuple, Value &value) const {
  RC rc = RC::SUCCESS;
  if (expr->type() == ExprType::FIELD || expr->type() == ExprType::VALUE) {
      rc = expr->get_value(tuple, value);
  } else if (expr->type() == ExprType::SUBQUERY) {
    auto sub_query = static_cast<SubQueryExpr *>(expr.get());
    rc = sub_query->get_one_row_value(tuple, value);
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
      } else if (right_->type() != ExprType::LIST) {
        return RC::SQL_SYNTAX;
      }
      
      while ((rc = right_->get_value(tuple, right_value)) == RC::SUCCESS) {
        if (left_value.compare(right_value) == 0) {
          is_in = true;
          break;
        }
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
    }
    break;
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
    }
    break;

    default: {
      // 正常就是原来的运算符，只不过左右表达式是子查询的时候，需要求一元表达式
      SubQueryExpr* sub_query = nullptr;

      // 获取左表达式的值
      if (left_->type() == ExprType::SUBQUERY) {
        sub_query = static_cast<SubQueryExpr*>(left_.get());
        rc = sub_query->get_one_row_value(tuple, left_value);
      } else {
        rc = left_->get_value(tuple, left_value);
      }
      if (rc != RC::SUCCESS) {
        LOG_WARN("左表达式非法");
        return rc;
      }
      // 获取右表达式的值
      if (right_->type() == ExprType::SUBQUERY) {
        sub_query = static_cast<SubQueryExpr*>(right_.get());
        rc = sub_query->get_one_row_value(tuple, right_value);
      } else {
        rc = right_->get_value(tuple, right_value);
      }
      if (rc != RC::SUCCESS) {
        LOG_WARN("failed to get_one_row. rc=%d:%s", rc, strrc(rc));
        return rc;
      }

      bool res;
      rc = compare_value(left_value, right_value, res);
      value.set_boolean(res);
    }
    break;
  }
  return rc;

}

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
          value.set_int(numeric_limits<int>::max());
        } else {
          value.set_int(left_value.get_int() / right_value.get_int());
        }
      } else {
        if (right_value.get_float() > -EPSILON && right_value.get_float() < EPSILON) {
          // NOTE:
          // 设置为浮点数最大值是不正确的。通常的做法是设置为NULL，但是当前的miniob没有NULL概念，所以这里设置为浮点数最大值。
          value.set_float(numeric_limits<float>::max());
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
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of left expression. rc=%s", strrc(rc));
    return rc;
  }
  rc = right_->get_value(tuple, right_value);
  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to get value of right expression. rc=%s", strrc(rc));
    return rc;
  }
  return calc_value(left_value, right_value, value);
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

////////////////////////////////////////////////////////////////////////////////
RC AggretationExpr::get_value(const Tuple &tuple, Value &value) const
{
  RC rc = RC::SUCCESS;
  switch (aggr_func_type_) {
    case AggrFuncType::MIN:
    case AggrFuncType::MAX:
    case AggrFuncType::SUM:
    case AggrFuncType::AVG: rc = tuple.find_cell(TupleCellSpec(field_.table_name(), field_.field_name()), value); break;
    case AggrFuncType::COUNT:
      rc = tuple.find_cell(TupleCellSpec(field_.table_name(), field_.field_name()), value);
      if (!value.is_null()) {
        value = Value(1);
      }
      break;
    case AggrFuncType::COUNT_STAR: value.set_int(1); break;
    default: break;
  }
  return rc;
}

////////////////////////////////////////////////////////////////////////////////

SubQueryExpr::SubQueryExpr() {}

SubQueryExpr::~SubQueryExpr()
{
  if (subquery_stmt != nullptr) {
    delete subquery_stmt;
  }
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

/**
 * @description: 如果返回的数据行数<=1, 则返回RC::SUCCESS
 * @param {Tuple} &tuple
 * @param {Value} &value
 * @return {*}
 */
RC SubQueryExpr::get_one_row_value(const Tuple &tuple, Value &value)
{
  RC rc = RC::SUCCESS;
  phy_oper->open(nullptr);
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


////////////////////////////////////////////////////////////////////////////////
RC ListExpr::get_value(const Tuple &tuple, Value &value) const {
  if (idx_ >= value_.size()) {
    return RC::EMPTY;
  }
  value = value_[idx_++];
  return RC::SUCCESS;
}