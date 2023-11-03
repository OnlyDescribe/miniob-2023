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
// Created by Wangyunlai on 2022/6/5.
//

#pragma once

#include <vector>
#include <memory>

#include "common/rc.h"
#include "sql/stmt/stmt.h"
#include "sql/stmt/join_on_stmt.h"
#include "storage/field/field.h"

class FieldMeta;
class FilterStmt;
class JoinOnStmt;
class Db;
class Table;

struct OrderByUnit
{
  Field field;
  SortType sort_type;
  OrderByUnit(const Field &other_field, const SortType &other_sort_type)
  {
    field = other_field;
    sort_type = other_sort_type;
  }
};

/**
 * @brief 表示select语句
 * @ingroup Statement
 */
class SelectStmt : public Stmt
{
public:
  SelectStmt() = default;
  ~SelectStmt() override;

  StmtType type() const override { return StmtType::SELECT; }

public:
  static RC create(Db *db, const SelectSqlNode &select_sql, Stmt *&stmt);
  static RC createField(
      const std::vector<Table *> &tables, const char *table_name, const char *attr_name, Field &field);

public:
  void set_is_aggregation_stmt(bool is_aggregation_stmt) { is_aggregation_stmt_ = is_aggregation_stmt; }
  bool is_aggregation_stmt() const { return is_aggregation_stmt_; }
  const std::vector<Table *> &tables() const { return tables_; }
  FilterStmt *filter_stmt() const { return filter_stmt_; }
  JoinOnStmt *join_on_stmt() const { return join_on_stmt_; }
  std::vector<OrderByUnit> *orderbys() const { return orderbys_.get(); }

  // not own this, move to physical operator
  std::vector<std::unique_ptr<Expression>> projects;
private:
  std::vector<Table *> tables_;
  std::unique_ptr<std::vector<OrderByUnit>> orderbys_;
  FilterStmt *filter_stmt_ = nullptr;
  JoinOnStmt *join_on_stmt_ = nullptr;
  bool is_aggregation_stmt_{false};
};
