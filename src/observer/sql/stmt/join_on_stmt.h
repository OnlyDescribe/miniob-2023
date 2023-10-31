/* Copyright (c) 2021 OceanBase and/or its affiliates. All rights reserved.
miniob is licensed under Mulan PSL v2.
You can use this software according to the terms and conditions of the Mulan PSL v2.
You may obtain a copy of Mulan PSL v2 at:
         http://license.coscl.org.cn/MulanPSL2
THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
See the Mulan PSL v2 for more details. */

#pragma once

#include <vector>
#include <unordered_map>
#include "sql/parser/parse_defs.h"
#include "sql/stmt/stmt.h"
#include "sql/expr/expression.h"

class Db;
class Table;
class FieldMeta;

struct JoinOnObj
{
  bool is_attr;
  Field field;
  Value value;

  void init_attr(const Field &field)
  {
    is_attr = true;
    this->field = field;
  }

  void init_value(const Value &value)
  {
    is_attr = false;
    this->value = value;
  }
};

// a Join b on a.id = b.id, 表1和表2,
// 可能会有 a.id = b.id and a.id > 3
// 过滤条件为a.id = b.id
class JoinOnUnit
{
public:
  JoinOnUnit() = default;
  ~JoinOnUnit() {}
  void set_comp(CompOp comp) { comp_ = comp; }
  CompOp comp() const { return comp_; }

  void set_left(const JoinOnObj &obj) { left_ = obj; }
  void set_right(const JoinOnObj &obj) { right_ = obj; }

  const JoinOnObj &left() const { return left_; }
  const JoinOnObj &right() const { return right_; }

private:
  CompOp comp_{CompOp::EQUAL_TO};
  JoinOnObj left_;
  JoinOnObj right_;
};

/**
 * @brief Join/谓词/过滤语句
 * @ingroup Statement
 * 含有多个表join的units
 */
class JoinOnStmt
{
public:
  JoinOnStmt() = default;
  virtual ~JoinOnStmt();

public:
  const std::vector<std::vector<JoinOnUnit *>> &join_units() const { return join_units_; }

public:
  static RC create(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      std::vector<PConditionExpr *> conditions, JoinOnStmt *&stmt);

  static RC create_join_unit(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      PConditionExpr *condition, JoinOnUnit *&join_unit);

private:
  static RC get_table_and_field(Db *db, Table *default_table, std::unordered_map<std::string, Table *> *tables,
      const RelAttrSqlNode &attr, Table *&table, const FieldMeta *&field);

private:
  std::vector<std::vector<JoinOnUnit *>> join_units_;  // 默认当前都是AND关系
};
