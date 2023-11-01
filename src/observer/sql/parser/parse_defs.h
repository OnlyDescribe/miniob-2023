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
// Created by Meiyi
//

#pragma once

#include <stddef.h>
#include <memory>
#include <vector>
#include <string>

#include "sql/parser/value.h"

class Expression;

// 1. 定义 ParsedExpression
// 与 Expression 的子类大致上对应, 为了不与expression.h中的类冲突, 类名加一个P(arsed)

// 1.1 将涉及到值字段、过滤条件、函数、子查询、四则运算等所复合形成的表达式统一
struct PUnaryExpr;
struct PArithmeticExpr;
struct ConditionSqlNode;
struct PFuncExpr;
struct PSubQueryExpr;
struct PListExpr;

using PConditionExpr = struct ConditionSqlNode;

enum class PExpType
{
  UNARY,       // 值、字段
  ARITHMETIC,  // 四则运算
  COMPARISON,  // 过滤条件
  FUNC,        // 函数
  AGGRFUNC,    // 聚合
  SUBQUERY,    // 子查询
  LIST,        // 值列表
};
// TODO: 析构函数or智能指针, 释放Expr*资源
struct PExpr
{
  PExpType type;
  PUnaryExpr *uexp;
  PArithmeticExpr *aexp;
  PConditionExpr *cexp;
  PFuncExpr *fexp;
  PSubQueryExpr *sexp;
  PListExpr *lexp;

  std::string name;
  std::string alias;

  PExpr() = default;
  PExpr(PUnaryExpr *exp)
  {
    type = PExpType::UNARY;
    uexp = exp;
  }
  PExpr(PArithmeticExpr *exp)
  {
    type = PExpType::ARITHMETIC;
    aexp = exp;
  }
  PExpr(PConditionExpr *exp)
  {
    type = PExpType::COMPARISON;
    cexp = exp;
  }
  PExpr(PFuncExpr *exp)
  {
    type = PExpType::FUNC;
    fexp = exp;
  }
  PExpr(PSubQueryExpr *exp)
  {
    type = PExpType::SUBQUERY;
    sexp = exp;
  }
  PExpr(PListExpr *exp)
  {
    type = PExpType::LIST;
    lexp = exp;
  }
};

// 1.2. 定义 ParsedExpression 中对象
// 1.2.1 一元表达式, 即值或字段
// 聚合类型
enum class AggrFuncType
{
  INVALID,
  MAX,
  MIN,
  SUM,
  AVG,
  COUNT,
  COUNT_STAR
};

// 字段(column, field)
struct RelAttrSqlNode
{
  std::string relation_name;                       ///< relation name (may be NULL) 表名
  std::string attribute_name;                      ///< attribute name              属性名
  AggrFuncType aggr_type = AggrFuncType::INVALID;  // 聚合类型
  std::vector<std::string> aggregates;             // 聚合字段
};

// 一元表达式
struct PUnaryExpr
{
  int is_attr;          ///< TRUE时，操作符是属性名，FALSE时，是属性值
  Value value;          ///< value if left_is_attr = FALSE
  RelAttrSqlNode attr;  ///< attribute
};

// 1.2.2 二元表达式, 即四则运算表达式
enum class PArithmeticType
{
  ADD,
  SUB,
  MUL,
  DIV,
  NEGATIVE,
};
struct PArithmeticExpr
{
  PArithmeticType type;
  PExpr *left;
  PExpr *right;
  PArithmeticExpr(PArithmeticType atype, PExpr *L, PExpr *R) : type(atype), left(L), right(R) {}
};

// 1.2.3 比较表达式
// 描述比较运算符
enum class CompOp
{
  EQUAL_TO,     ///< "="
  LESS_EQUAL,   ///< "<="
  NOT_EQUAL,    ///< "<>"
  LESS_THAN,    ///< "<"
  GREAT_EQUAL,  ///< ">="
  GREAT_THAN,   ///< ">"
  LIKE,         ///< "like"
  NOT_LIKE,     ///< "not like"
  IS_NULL,      ///< "is null"
  IS_NOT_NULL,  ///< "is not null"
  IN,           ///< "sub_query in"
  NOT_IN,       ///< "sub_query not in"
  EXISTS,       ///< "sub_query exists"
  NOT_EXISTS,   ///< "sub_query not exists"
  AND,          ///< "condition and"
  OR,           ///< "condition or"
  NO_OP
};

// 条件比较
// 左边和右边理论上都可以是任意的数据，比如是字段（属性，列），也可以是数值常量。
struct ConditionSqlNode
{
  // 这里的value/attr我们抽象成一元表达式
  CompOp comp;  ///< comparison operator
  PExpr *left;
  PExpr *right;
  ConditionSqlNode(CompOp op, PExpr *L, PExpr *R) : comp(op), left(L), right(R) {}
};

// 1.2.4 Function
enum class PFuncType
{
  LENGTH,
  ROUND,
  DATE_FORMAT,
};
struct PFuncExpr
{
  PFuncType type;
  std::vector<PExpr *> params;
};

// 1.2.5 子查询
struct SelectSqlNode;
struct PSubQueryExpr
{
  SelectSqlNode *sub_select;
};

// 1.2.6 值列表
struct PListExpr
{
  std::vector<Value> value_list;
};

// 2. 定义 Select， 即SelectSqlNode

// 2.1 Select
struct SelectAttr;
struct Relation;
struct OrderBy;
// struct GroupBy;

struct SelectSqlNode
{
  // in select clause
  std::vector<PExpr *> attributes;

  // in from clause
  std::vector<Relation> relations;           ///< 查询的表
  std::vector<PConditionExpr *> join_conds;  ///< 连接条件
                                             ///< Expr得是指针, 否则在解析阶段会delete, 难通过析构来管理内存
  // in where clause
  PConditionExpr *conditions;

  // in groupby clause
  std::vector<RelAttrSqlNode> groupbys;
  PConditionExpr *havings;

  // in orderby clause
  std::vector<OrderBy> orderbys;
};

// 2.2 Select 中对象的定义
// 2.2.1 in select clause
// struct SelectAttr  // select的字段/子表达式
// {
//   PExpr *expr;               ///< 语义上返回单行的表达式
//   RelAttrSqlNode attribute;  ///< xxx.xxx形式的字段(包括聚合)
// };

// 2.2.2 in from clause
struct Relation
{
  std::string relation_name;
  std::string alias;
};
// 把 SelectSqlNode 所有表join起来的条件, N 个表，N-1个条件
struct FromSqlNode
{
  std::vector<Relation> relations;           // 需要join的表
  std::vector<PConditionExpr *> join_conds;  // join的条件
  // join 目前只需要支持 and, 故实际上PConditionExpr以及其子树的CompOp唯一
};

// 2.2.3 in where clause
// 排序类型
enum class SortType
{
  ASC,
  DESC
};

// OrderBy单元
struct OrderBy
{
  RelAttrSqlNode attr;  // order_by的属性
  SortType sort_type;   // 排序类型,
};

/**
 * @brief 算术表达式计算的语法树
 * @ingroup SQLParser
 */
struct CalcSqlNode
{
  std::vector<Expression *> expressions;  ///< calc clause

  ~CalcSqlNode();
};

/**
 * @brief 描述一个insert语句
 * @ingroup SQLParser
 * @details 于Selects类似，也做了很多简化
 */
struct InsertSqlNode
{
  std::string relation_name;  ///< Relation to insert into
  std::vector<Value> values;  ///< 要插入的值
};

/**
 * @brief 描述一个delete语句
 * @ingroup SQLParser
 */
struct DeleteSqlNode
{
  std::string relation_name;  ///< Relation to delete from
  PConditionExpr *conditions;
};

// 表示 Update 语句中 SET 的赋值
struct AssignmentSqlNode
{
  std::string attribute_name;  ///< 属性名, 因为只支持单表, 不需要RelAttrSqlNode类型
  bool is_value;               ///< 赋值
  Value value;                 ///< 值
  PExpr *expr;                 ///< 支持表达式
};

/**
 * @brief 描述一个update语句
 * @ingroup SQLParser
 */
struct UpdateSqlNode
{
  std::string relation_name;  ///< Relation to update
  std::vector<AssignmentSqlNode> assignments;
  PConditionExpr *conditions;
};

/**
 * @brief 描述一个属性
 * @ingroup SQLParser
 * @details 属性，或者说字段(column, field)
 * Rel -> Relation
 * Attr -> Attribute
 */
struct AttrInfoSqlNode
{
  AttrType type;     ///< Type of attribute
  std::string name;  ///< Attribute name
  size_t length;     ///< Length of attribute
  bool is_not_null = false;
};

/**
 * @brief 描述一个create table语句
 * @ingroup SQLParser
 * @details 这里也做了很多简化。
 */
struct CreateTableSqlNode
{
  std::string relation_name;                ///< Relation name
  std::vector<AttrInfoSqlNode> attr_infos;  ///< attributes
};

/**
 * @brief 描述一个drop table语句
 * @ingroup SQLParser
 */
struct DropTableSqlNode
{
  std::string relation_name;  ///< 要删除的表名
};

/**
 * @brief 描述一个create index语句
 * @ingroup SQLParser
 * @details 创建索引时，需要指定索引名，表名，字段名。
 */
struct CreateIndexSqlNode
{
  bool is_unique;
  std::string index_name;                   ///< Index name
  std::string relation_name;                ///< Relation name
  std::vector<std::string> attribute_name;  ///< Attribute name
};

/**
 * @brief 描述一个drop index语句
 * @ingroup SQLParser
 */
struct DropIndexSqlNode
{
  std::string index_name;     ///< Index name
  std::string relation_name;  ///< Relation name
};

/**
 * @brief 描述一个desc table语句
 * @ingroup SQLParser
 * @details desc table 是查询表结构信息的语句
 */
struct DescTableSqlNode
{
  std::string relation_name;
};

/**
 * @brief 描述一个load data语句
 * @ingroup SQLParser
 * @details 从文件导入数据到表中。文件中的每一行就是一条数据，每行的数据类型、字段个数都与表保持一致
 */
struct LoadDataSqlNode
{
  std::string relation_name;
  std::string file_name;
};

/**
 * @brief 设置变量的值
 * @ingroup SQLParser
 * @note 当前还没有查询变量
 */
struct SetVariableSqlNode
{
  std::string name;
  Value value;
};

class ParsedSqlNode;

/**
 * @brief 描述一个explain语句
 * @ingroup SQLParser
 * @details 会创建operator的语句，才能用explain输出执行计划。
 * 一个command就是一个语句，比如select语句，insert语句等。
 * 可能改成SqlCommand更合适。
 */
struct ExplainSqlNode
{
  std::unique_ptr<ParsedSqlNode> sql_node;
};

/**
 * @brief 解析SQL语句出现了错误
 * @ingroup SQLParser
 * @details 当前解析时并没有处理错误的行号和列号
 */
struct ErrorSqlNode
{
  std::string error_msg;
  int line;
  int column;
};

/**
 * @brief 表示一个SQL语句的类型
 * @ingroup SQLParser
 */
enum SqlCommandFlag
{
  SCF_ERROR = 0,
  SCF_INVALID,
  SCF_CALC,
  SCF_SELECT,
  SCF_INSERT,
  SCF_UPDATE,
  SCF_DELETE,
  SCF_CREATE_TABLE,
  SCF_DROP_TABLE,
  SCF_CREATE_INDEX,
  SCF_DROP_INDEX,
  SCF_SYNC,
  SCF_SHOW_TABLES,
  SCF_DESC_TABLE,
  SCF_BEGIN,  ///< 事务开始语句，可以在这里扩展只读事务
  SCF_COMMIT,
  SCF_CLOG_SYNC,
  SCF_ROLLBACK,
  SCF_LOAD_DATA,
  SCF_HELP,
  SCF_EXIT,
  SCF_EXPLAIN,
  SCF_SET_VARIABLE,  ///< 设置变量
};
/**
 * @brief 表示一个SQL语句
 * @ingroup SQLParser
 */
class ParsedSqlNode
{
public:
  enum SqlCommandFlag flag;
  ErrorSqlNode error;
  CalcSqlNode calc;
  SelectSqlNode selection;
  InsertSqlNode insertion;
  DeleteSqlNode deletion;
  UpdateSqlNode update;
  CreateTableSqlNode create_table;
  DropTableSqlNode drop_table;
  CreateIndexSqlNode create_index;
  DropIndexSqlNode drop_index;
  DescTableSqlNode desc_table;
  LoadDataSqlNode load_data;
  ExplainSqlNode explain;
  SetVariableSqlNode set_variable;

public:
  ParsedSqlNode();
  explicit ParsedSqlNode(SqlCommandFlag flag);
};

/**
 * @brief 表示语法解析后的数据
 * @ingroup SQLParser
 */
class ParsedSqlResult
{
public:
  void add_sql_node(std::unique_ptr<ParsedSqlNode> sql_node);
  std::vector<std::unique_ptr<ParsedSqlNode>> &sql_nodes() { return sql_nodes_; }

private:
  std::vector<std::unique_ptr<ParsedSqlNode>> sql_nodes_;  ///< 这里记录SQL命令。虽然看起来支持多个，但是当前仅处理一个
};
