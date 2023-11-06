#pragma once

#include <string>
#include <vector>

#include "sql/stmt/stmt.h"
#include "select_stmt.h"

class Db;

class CreateViewStmt : public Stmt
{
public:
  CreateViewStmt() = default;
  virtual ~CreateViewStmt() = default;

  StmtType type() const override { return StmtType::CREATE_VIEW; }

  const std::string &table_name() const { return table_name_; }

  static RC create(Db *db, const CreateViewSqlNode &create_view_sql, Stmt *&stmt);

  const std::vector<std::string> &alias() const { return alias_; }
  std::unique_ptr<SelectStmt> &select_stmt() { return select_stmt_; }

private:
  std::string table_name_;
  std::vector<std::string> alias_;
  std::unique_ptr<SelectStmt> select_stmt_;
};