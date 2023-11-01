#pragma once

#include <string>
#include <vector>

#include "sql/stmt/stmt.h"
#include "select_stmt.h"

class Db;

class CreateTableSelectStmt : public Stmt
{
public:
  CreateTableSelectStmt() = default;
  virtual ~CreateTableSelectStmt() = default;

  StmtType type() const override { return StmtType::CREATE_TABLE_SELECT; }

  const std::string &table_name() const { return table_name_; }

  static RC create(Db *db, const CreateTableSelectSqlNode &create_table_select_sql, Stmt *&stmt);

  const std::vector<AttrInfoSqlNode> &attr_infos() const { return attr_infos_; }
  std::unique_ptr<SelectStmt> &select_stmt() { return select_stmt_; }

private:
  std::string table_name_;
  std::vector<AttrInfoSqlNode> attr_infos_;
  std::unique_ptr<SelectStmt> select_stmt_;
};