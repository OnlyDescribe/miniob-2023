#include "sql/stmt/create_view_stmt.h"
#include "event/sql_debug.h"
#include "sql/expr/expression.h"

RC CreateViewStmt::create(Db *db, const CreateViewSqlNode &create_view, Stmt *&stmt)
{
  RC rc = RC::SUCCESS;

  CreateViewStmt *create_view_stmt = new CreateViewStmt();
  create_view_stmt->table_name_ = create_view.relation_name;
  //   create_view_stmt->attr_infos_ = create_table_select.attr_infos;

  Stmt *select_stmt = nullptr;
  rc = SelectStmt::create(db, create_view.select, select_stmt);
  create_view_stmt->select_stmt_.reset(static_cast<SelectStmt *>(select_stmt));

  stmt = create_view_stmt;
  return rc;
}
