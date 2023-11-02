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
// Created by Wangyunlai on 2022/5/22.
//

#include "sql/stmt/update_stmt.h"
#include "sql/stmt/select_stmt.h"
#include "common/log/log.h"
#include "sql/stmt/filter_stmt.h"
#include "storage/db/db.h"

UpdateStmt::UpdateStmt(Table *table, std::vector<std::unique_ptr<Expression>> &&values_exprs,
    const std::vector<const FieldMeta *> &field_metas, FilterStmt *filter_stmt)
    : table_(table), value_exprs_(std::move(values_exprs)), field_metas_(field_metas), filter_stmt_(filter_stmt)
{}

RC UpdateStmt::create(Db *db, UpdateSqlNode &update, Stmt *&stmt)
{
  const char *table_name = update.relation_name.c_str();
  if (nullptr == db || nullptr == table_name) {
    LOG_WARN("invalid argument. db=%p, table_name=%p", db, table_name);
    return RC::INVALID_ARGUMENT;
  }

  // 0. 找到对应的table
  Table *table = db->find_table(table_name);  // 只需要支持单表
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // 1. 处理更新字段
  // std::vector<const Value *> values;
  std::vector<std::unique_ptr<Expression>> value_exprs;
  std::vector<const FieldMeta *> field_metas;

  const int value_amount = update.assignments.size();

  const TableMeta &table_meta = table->table_meta();
  const int sys_field_num = table_meta.sys_field_num();
  const int field_num = table_meta.field_num() - table_meta.extra_field_num();

  // 检查表中是否存在相应字段, 设置field_metas和values
  for (int i = 0; i < value_amount; ++i) {
    const std::string &attribute_name = update.assignments[i].attribute_name;

    if (attribute_name.empty()) {
      LOG_WARN("invalid argument");
      return RC::INVALID_ARGUMENT;
    }

    bool field_found{false};
    for (int j = sys_field_num; j < field_num; j++) {
      if (0 != strcmp(table_meta.field(j)->name(), attribute_name.c_str())) {
        continue;
      } else {
        field_found = true;
        field_metas.push_back(table_meta.field(j));
        break;
      }
    }

    if (!field_found) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    // 如果右赋值是UNARY
    // 检查字段和值类型是否冲突
    ASSERT(!field_metas.empty(), "");
    if (update.assignments[i].expr->type == PExpType::UNARY) {
      if (update.assignments[i].expr->uexp->is_attr) {
        LOG_WARN("等号右边不应该是attr类型");
        return RC::SQL_SYNTAX;
      }
    }

#if 0
    // 下面的Resolve需要放到update的物理算子
    // 尽可能解决Unary表达式与字段类型不统一的冲突
    const FieldMeta *field_meta = field_metas.back();
    const AttrType value_type = update.assignments[i].expr->uexp->value.attr_type();
    const AttrType field_type = field_meta->type();

    if (field_type != value_type && update.assignments[i].expr->type == PExpType::UNARY) {
      // 1) 因为更新操作的词法解析无法判断字符串是TEXTS还是CHARS
      // 目前可能会出现值 TEXTS 类型而字段是 CHARS 类型
      if (field_type == AttrType::TEXTS && value_type == AttrType::CHARS) {
        update.assignments[i].expr->uexp->value.set_type(AttrType::TEXTS);
      }
      // 2) 如果值为 NULL, 判断该字段是否设置了 NOT NULL
      else if (value_type == AttrType::NULLS) {
        if (field_meta->is_not_null()) {
          LOG_WARN("value can not be null. table=%s, field=%s, field type=%d, value_type=%d",
          table_name, field_meta->name(), field_type, value_type);
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      }
      // 3) 处理INTS, FLOATS和CHARS之间的类型转换
      else if ((field_type == AttrType::INTS || field_type == AttrType::FLOATS || field_type == AttrType::CHARS) and
               (value_type == AttrType::INTS || value_type == AttrType::FLOATS || value_type == AttrType::CHARS)) {
        Value &v = update.assignments[i].expr->uexp->value;
        switch (field_type) {
            // 注意 floats 要截断值; CHARS 若是纯数字同样阶段转换值, 否则报错
          case AttrType::INTS: {
            v = Value(v.get_int());
          } break;
          case AttrType::FLOATS: {
            v = Value(v.get_float());
          } break;
          case AttrType::CHARS: {
            v = Value(v.get_string().data());
          } break;
          default: {
            LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
          table_name, field_meta->name(), field_type, value_type);
            return RC::SCHEMA_FIELD_TYPE_MISMATCH;
          } break;
        }
      }

      else {
        LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
          table_name, field_meta->name(), field_type, value_type);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
#endif
    // 处理 value_exprs
    if (update.assignments[i].expr->type == PExpType::UNARY) {
      value_exprs.emplace_back(new ValueExpr(update.assignments[i].expr->uexp->value));
    } else if (update.assignments[i].expr->type == PExpType::SUBQUERY) {
      auto &sub_select = update.assignments[i].expr->sexp;
      Stmt *subquery_stmt = nullptr;
      RC rc = SelectStmt::create(db, *sub_select->sub_select, subquery_stmt);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      auto sub_query_expr = new SubQueryExpr();
      sub_query_expr->subquery_stmt = static_cast<SelectStmt *>(subquery_stmt);
      value_exprs.emplace_back(sub_query_expr);
    } else {
      LOG_WARN("暂时不支持的类型");
      return RC::SQL_SYNTAX;
    }
  }
  // 2. 处理过滤条件
  std::unordered_map<std::string, Table *> table_map;
  table_map.insert(std::pair<std::string, Table *>(std::string(table_name), table));
  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;

  RC rc = FilterStmt::create(db, table, &table_map, update.conditions, filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // 3. everything alright
  stmt = new UpdateStmt(table, std::move(value_exprs), field_metas, filter_stmt);
  return RC::SUCCESS;
}
