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
// Created by Wangyunlai on 2023/6/13.
//

#include "sql/stmt/create_table_select_stmt.h"
#include "event/sql_debug.h"
#include "sql/expr/expression.h"

RC CreateTableSelectStmt::create(Db *db, const CreateTableSelectSqlNode &create_table_select, Stmt *&stmt)
{
  CreateTableSelectStmt *create_table_select_stmt = new CreateTableSelectStmt();
  create_table_select_stmt->table_name_ = create_table_select.relation_name;

  Stmt *select_stmt = nullptr;
  SelectStmt::create(db, create_table_select.select, select_stmt);
  create_table_select_stmt->select_stmt_.reset(static_cast<SelectStmt *>(select_stmt));

  stmt = create_table_select_stmt;
  return RC::SUCCESS;
}
