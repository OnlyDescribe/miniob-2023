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
// Created by Wangyunlai on 2023/4/25.
//

#include "sql/executor/create_index_executor.h"
#include "sql/stmt/create_index_stmt.h"
#include "event/sql_event.h"
#include "event/session_event.h"
#include "session/session.h"
#include "common/log/log.h"
#include "storage/table/table.h"

RC CreateIndexExecutor::execute(SQLStageEvent *sql_event)
{
  Stmt *stmt = sql_event->stmt();
  Session *session = sql_event->session_event()->session();
  ASSERT(stmt->type() == StmtType::CREATE_INDEX,
      "create index executor can not run this command: %d",
      static_cast<int>(stmt->type()));

  CreateIndexStmt *create_index_stmt = static_cast<CreateIndexStmt *>(stmt);

  Trx *trx = session->current_trx();
  Table *table = create_index_stmt->table();

  // 这里从db复制了一份字段field_metas(一般字段名不大)
  // 这是考虑到尽可能少修改并统一联合索引字段的接口, 即 const std::vector<FieldMeta> &field_meta
  std::vector<FieldMeta> new_field_metas;
  const std::vector<const FieldMeta *> &field_metas = create_index_stmt->field_meta();
  new_field_metas.reserve(field_metas.size());
  for (const FieldMeta *field_meta_ptr : field_metas) {
    new_field_metas.push_back(*field_meta_ptr);
  }

  return table->create_index(
      trx, new_field_metas, create_index_stmt->index_name().c_str(), create_index_stmt->is_unique());
}