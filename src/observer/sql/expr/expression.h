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

#pragma once

#include <string.h>
#include <memory>
#include <unordered_map>
#include <string>
#include <utility>

#include "storage/field/field.h"
#include "sql/parser/value.h"
#include "common/log/log.h"
#include "storage/record/record.h"

class Tuple;
class SelectStmt;
class Stmt;
class LogicalOperator;
class PhysicalOperator;
class Db;

const static std::unordered_map<AggrFuncType, std::string> AggretationExprStr = {
    {AggrFuncType::MAX, "MAX"},
    {AggrFuncType::MIN, "MIN"},
    {AggrFuncType::AVG, "AVG"},
    {AggrFuncType::SUM, "SUM"},
    {AggrFuncType::COUNT, "COUNT"},
    {AggrFuncType::COUNT_STAR, "COUNT"},
};

/**
 * @defgroup Expression
 * @brief 表达式
 */

/**
 * @brief 表达式类型
 * @ingroup Expression
 */
enum class ExprType
{
  NONE,
  STAR,          ///< 星号，表示所有字段
  FIELD,         ///< 字段。在实际执行时，根据行数据内容提取对应字段的值
  VALUE,         ///< 常量值
  AGGRFUNCTION,  /// < 聚合表达式
  CAST,          ///< 需要做类型转换的表达式
  COMPARISON,    ///< 需要做比较的表达式
  CONJUNCTION,   ///< 多个表达式使用同一种关系(AND或OR)来联结
  ARITHMETIC,    ///< 算术运算
  SUBQUERY,      ///< 子查询
  LIST,          ///< 列表达式
  HavingField,   ///< 取得havingField的值
  FUNCTION       ///< 取得havingField的值
};

/**
 * @brief 表达式的抽象描述
 * @ingroup Expression
 * @details 在SQL的元素中，任何需要得出值的元素都可以使用表达式来描述
 * 比如获取某个字段的值、比较运算、类型转换
 * 当然还有一些当前没有实现的表达式，比如算术运算。
 *
 * 通常表达式的值，是在真实的算子运算过程中，拿到具体的tuple后
 * 才能计算出来真实的值。但是有些表达式可能就表示某一个固定的
 * 值，比如ValueExpr。
 */
class Expression
{
public:
  Expression() = default;
  virtual ~Expression() = default;

  /**
   * @brief 根据具体的tuple，来计算当前表达式的值。tuple有可能是一个具体某个表的行数据
   */
  virtual RC get_value(const Tuple &tuple, Value &value) const = 0;

  /**
   * @brief 根据具体的tuple，来计算当前表达式的值所在的 Record, 如果不存在则返回 NOTFOUND
   */
  virtual RC get_record(const Tuple &tuple, RecordPos &rid) const = 0;

  /**
   * @brief 在没有实际运行的情况下，也就是无法获取tuple的情况下，尝试获取表达式的值
   * @details 有些表达式的值是固定的，比如ValueExpr，这种情况下可以直接获取值
   */
  virtual RC try_get_value(Value &value) const { return RC::UNIMPLENMENT; }

  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  virtual ExprType type() const = 0;

  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  virtual AttrType value_type() const = 0;

  virtual std::string to_string()
  {
    switch (type()) {
      case ExprType::STAR: return "STAR";
      case ExprType::FIELD: return "FIELD";
      case ExprType::VALUE: return "VALUE";
      case ExprType::AGGRFUNCTION: return "AGGRFUNCTION";
      case ExprType::CAST: return "CAST";
      case ExprType::COMPARISON: return "COMPARISON";
      case ExprType::CONJUNCTION: return "CONJUNCTION";
      case ExprType::ARITHMETIC: return "ARITHMETIC";
      case ExprType::SUBQUERY: return "SUBQUERY";
      default: break;
    }
    return "NOT SUPPORT";
  }

  /**
   * @brief 表达式的名字，比如是字段名称，或者用户在执行SQL语句时输入的内容
   */
  virtual std::string name() const { return name_; }
  virtual void set_name(std::string name) { name_ = std::move(name); }
  virtual void set_name(const std::string &name, const std::string &second) { name_ = name + "." + second; }
  static RC create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
      const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp = CompOp::NO_OP,
      Db *db = nullptr);

private:
  std::string name_;
};

/**
 * @brief 字段表达式
 * @ingroup Expression
 */
class FieldExpr : public Expression
{
public:
  FieldExpr() = default;
  FieldExpr(const Table *table, const FieldMeta *field) : field_(table, field) {}
  FieldExpr(const Field &field) : field_(field) {}

  virtual ~FieldExpr() = default;

  ExprType type() const override { return ExprType::FIELD; }
  AttrType value_type() const override { return field_.attr_type(); }

  Field &field() { return field_; }

  const Field &field() const { return field_; }

  const char *table_name() const { return field_.table_name(); }

  const char *field_name() const { return field_.field_name(); }

  RC get_value(const Tuple &tuple, Value &value) const override;

  RC get_record(const Tuple &tuple, RecordPos &rid) const override;

  static void get_fieldexprs(Expression *expr, std::vector<Expression *> &field_exprs);
  static RC create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
      const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp = CompOp::NO_OP,
      Db *db = nullptr);

private:
  Field field_;
};

/**
 * @brief 常量值表达式
 * @ingroup Expression
 */
class ValueExpr : public Expression
{
public:
  ValueExpr() = default;
  explicit ValueExpr(const Value &value) : value_(value) {}

  virtual ~ValueExpr() = default;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override
  {
    value = value_;
    return RC::SUCCESS;
  }

  RC get_record(const Tuple &tuple, RecordPos &rid) const override;

  ExprType type() const override { return ExprType::VALUE; }

  AttrType value_type() const override { return value_.attr_type(); }

  void get_value(Value &value) const { value = value_; }

  const Value &get_value() const { return value_; }

  static RC create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
      const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp = CompOp::NO_OP,
      Db *db = nullptr);

private:
  Value value_;
};

/**
 * @brief 类型转换表达式
 * @ingroup Expression
 */
class CastExpr : public Expression
{
public:
  CastExpr(std::unique_ptr<Expression> child, AttrType cast_type);
  virtual ~CastExpr();

  ExprType type() const override { return ExprType::CAST; }
  RC get_value(const Tuple &tuple, Value &value) const override;

  RC try_get_value(Value &value) const override;

  RC get_record(const Tuple &tuple, RecordPos &rid) const override;

  AttrType value_type() const override { return cast_type_; }

  std::unique_ptr<Expression> &child() { return child_; }

private:
  RC cast(const Value &value, Value &cast_value) const;

private:
  std::unique_ptr<Expression> child_;  ///< 从这个表达式转换
  AttrType cast_type_;                 ///< 想要转换成这个类型
};

/**
 * @brief 比较表达式
 * @ingroup Expression
 */
class ComparisonExpr : public Expression
{
public:
  ComparisonExpr(CompOp comp, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ComparisonExpr();

  ExprType type() const override { return ExprType::COMPARISON; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  RC get_record(const Tuple &tuple, RecordPos &rid) const override;

  AttrType value_type() const override { return BOOLEANS; }

  bool is_like(const std::string &s, const std::string &pattern) const;

  CompOp comp() const { return comp_; }

  // 反转比较符号
  void reverse_comp();

  std::unique_ptr<Expression> &left() { return left_; }
  std::unique_ptr<Expression> &right() { return right_; }

  /**
   * 尝试在没有tuple的情况下获取当前表达式的值
   * 在优化的时候，可能会使用到
   */
  RC try_get_value(Value &value) const override;

  /**
   * compare the two tuple cells
   * @param value the result of comparison
   */
  RC compare_value(const Value &left, const Value &right, bool &value) const;

  void set_parent_tuple(const std::shared_ptr<Tuple> &tuple) { parent_tuple_ = tuple; }

private:
  // 返回表达式的值，必须为空或者单值类型
  RC get_one_row_value(const std::unique_ptr<Expression> &expr, const Tuple &tuple, Value &value) const;

  CompOp comp_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
  std::shared_ptr<Tuple> parent_tuple_;
};

/**
 * @brief 联结表达式
 * @ingroup Expression
 * 多个表达式使用同一种关系(AND或OR)来联结
 * 当前miniob仅有AND操作
 */
class ConjunctionExpr : public Expression
{
public:
  enum class Type
  {
    AND,
    OR,
  };

public:
  ConjunctionExpr(Type type, std::vector<std::unique_ptr<Expression>> &children);
  virtual ~ConjunctionExpr() = default;

  ExprType type() const override { return ExprType::CONJUNCTION; }

  AttrType value_type() const override { return BOOLEANS; }

  RC get_value(const Tuple &tuple, Value &value) const override;

  RC get_record(const Tuple &tuple, RecordPos &rid) const override;

  Type conjunction_type() const { return conjunction_type_; }

  void set_parent_tuple(const std::shared_ptr<Tuple> &tuple)
  {
    for (auto &child : children_) {
      if (child->type() == ExprType::COMPARISON) {
        auto cmp_expr = static_cast<ComparisonExpr *>(child.get());
        cmp_expr->set_parent_tuple(tuple);
      }
    }
  }

  std::vector<std::unique_ptr<Expression>> &children() { return children_; }

private:
  Type conjunction_type_;
  std::vector<std::unique_ptr<Expression>> children_;
};

/**
 * @brief 算术表达式
 * @ingroup Expression
 */
class ArithmeticExpr : public Expression
{
public:
  enum class Type
  {
    ADD,
    SUB,
    MUL,
    DIV,
    NEGATIVE,
  };

public:
  ArithmeticExpr(Type type, Expression *left, Expression *right);
  ArithmeticExpr(Type type, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right);
  virtual ~ArithmeticExpr() = default;

  ExprType type() const override { return ExprType::ARITHMETIC; }

  AttrType value_type() const override;

  RC get_value(const Tuple &tuple, Value &value) const override;
  RC try_get_value(Value &value) const override;

  RC get_record(const Tuple &tuple, RecordPos &rid) const override;

  Type arithmetic_type() const { return arithmetic_type_; }

  std::unique_ptr<Expression> &left() { return left_; }
  std::unique_ptr<Expression> &right() { return right_; }

  static RC create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
      const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp = CompOp::NO_OP,
      Db *db = nullptr);

private:
  RC calc_value(const Value &left_value, const Value &right_value, Value &value) const;

private:
  Type arithmetic_type_;
  std::unique_ptr<Expression> left_;
  std::unique_ptr<Expression> right_;
};

/**
 * @brief 聚合表达式
 * 表达对某个字段的聚合操作, 由Field和聚合类型两部分组成
 * 表示对每一个tuple进行聚合操作后的结果
 */
class AggretationExpr : public Expression
{
public:
  // AggretationExpr() = default; 注意如果类型为star, 那么expr是空指针
  explicit AggretationExpr(Expression *expr, AggrFuncType aggr_func_type, bool is_star)
      : aggr_func_type_(aggr_func_type), is_star_(is_star)
  {
    field_expr_.reset(expr);
    value_.set_null();
  }
  explicit AggretationExpr(std::unique_ptr<Expression> &&expr, AggrFuncType aggr_func_type, bool is_star)
      : aggr_func_type_(aggr_func_type), is_star_(is_star), field_expr_(std::move(expr))
  {
    value_.set_null();
  }
  RC get_value(const Tuple &tuple, Value &value) const;

  RC try_get_value(Value &value) const { return RC::UNIMPLENMENT; }

  RC get_record(const Tuple &tuple, RecordPos &rid) const;

  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  ExprType type() const { return ExprType::AGGRFUNCTION; }

  void set_value(const Value &value)
  {
    has_value = true;
    value_ = value;
  }

  void clear_value() { has_value = false; }

  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  AttrType value_type() const { return field_expr_->value_type(); };

  AggrFuncType aggr_type() { return aggr_func_type_; }

  static void get_aggrfuncexprs(Expression *expr, std::vector<Expression *> &aggrfunc_exprs);
  static RC create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
      const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp = CompOp::NO_OP,
      Db *db = nullptr);

private:
  AggrFuncType aggr_func_type_;
  bool is_star_{false};
  // 一般是field表达式
  std::unique_ptr<Expression> field_expr_;

  // 聚合后的值
  Value value_;
  bool has_value{false};
};

/**
 * @description: 子查询表达式，可以返回一行或者多行
 * @return {*}
 */
class SubQueryExpr : public Expression
{
public:
  SubQueryExpr();
  virtual ~SubQueryExpr();

  virtual RC get_value(const Tuple &tuple, Value &value) const;
  // tuple和parent需要组合, 用于复杂子查询
  RC get_and_set_value(const Tuple &tuple, Value &value, Tuple *parent = nullptr) const;

  // 从子查询里面，确保只能拿一个记录
  RC get_one_row_value(const Tuple &tuple, Value &value);

  // tuple和parent需要组合, 用于复杂子查询
  RC get_and_set_one_row_value(const Tuple &tuple, Value &value, Tuple *parent = nullptr);

  virtual RC try_get_value(Value &value) const { return RC::UNIMPLENMENT; }

  virtual RC get_record(const Tuple &tuple, RecordPos &rid) const;

  virtual ExprType type() const { return ExprType::SUBQUERY; }

  virtual AttrType value_type() const { return AttrType::UNDEFINED; }

  static RC create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
      const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp = CompOp::NO_OP,
      Db *db = nullptr);

  // should own this?
  SelectStmt *subquery_stmt{nullptr};
  std::unique_ptr<LogicalOperator> oper;
  std::unique_ptr<PhysicalOperator> phy_oper;
};

/**
 * @description: List表达式，形如(1,2,3,4)
 * @return {*}
 */

class ListExpr : public Expression
{
public:
  ListExpr() = default;

  virtual ~ListExpr() = default;

  virtual RC get_value(const Tuple &tuple, Value &value) const;

  virtual RC try_get_value(Value &value) const { return RC::UNIMPLENMENT; }

  virtual RC get_record(const Tuple &tuple, RecordPos &rid) const;

  virtual ExprType type() const { return ExprType::LIST; }

  Expression *expr_at(int i) { return values_[i].get(); }

  virtual AttrType value_type() const
  {
    if (values_.empty()) {
      return AttrType::NULLS;
    }
    return values_[0]->value_type();
  }

  void reset() { idx_ = 0; }

  // 移动到values里面取
  void set_values(std::vector<std::unique_ptr<Expression>> &&values) { values_ = std::move(values); }

  static RC create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
      const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp = CompOp::NO_OP,
      Db *db = nullptr);

private:
  std::vector<std::unique_ptr<Expression>> values_;
  mutable int idx_{0};
};

/**
 * @brief having表达式，用于从聚合的完整列中，取值
 * 比如 having a > 1 and b < 1;
 * get_value() 能返回a的值
 */
class HavingFieldExpr : public Expression
{
public:
  explicit HavingFieldExpr(int pos) : pos_(pos) {}

  virtual ~HavingFieldExpr() = default;

  virtual RC get_value(const Tuple &tuple, Value &value) const;

  RC try_get_value(Value &value) const { return RC::UNIMPLENMENT; }

  virtual RC get_record(const Tuple &tuple, RecordPos &rid) const;
  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  ExprType type() const { return ExprType::HavingField; }

  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  AttrType value_type() const { return AttrType::UNDEFINED; };

private:
  int pos_;
};

/**
 * @brief function表达式, length、round和date_format。
 * 比如 having a > 1 and b < 1;
 */
class FunctionExpr : public Expression
{
public:
  explicit FunctionExpr(PFuncType type, Expression *param1, Expression *param2 = nullptr)
      : type_(type), param1_(param1), param2_(param2)
  {}

  virtual ~FunctionExpr() = default;

  virtual RC get_value(const Tuple &tuple, Value &value) const;

  RC try_get_value(Value &value) const { return RC::UNIMPLENMENT; }

  virtual RC get_record(const Tuple &tuple, RecordPos &rid) const;

  /**
   * @brief 表达式的类型
   * 可以根据表达式类型来转换为具体的子类
   */
  ExprType type() const { return ExprType::FUNCTION; }
  /**
   * @brief 表达式值的类型
   * @details 一个表达式运算出结果后，只有一个值
   */
  AttrType value_type() const { return AttrType::UNDEFINED; };

  static RC create_expression(const PExpr *expr, const std::unordered_map<std::string, Table *> &parent_table_map,
      const std::unordered_map<std::string, Table *> &cur_table_map, Expression *&res_expr, CompOp comp = CompOp::NO_OP,
      Db *db = nullptr);

private:
  RC date_format(const Tuple &tuple, Value &value) const;
  RC length(const Tuple &tuple, Value &value) const;
  RC round(const Tuple &tuple, Value &value) const;

private:
  PFuncType type_;
  Expression *param1_;
  Expression *param2_;
};