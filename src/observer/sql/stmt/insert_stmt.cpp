/* Copyright (c) 2021OceanBase and/or its affiliates. All rights reserved.
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

#include "sql/stmt/insert_stmt.h"
#include "sql/expr/expression.h"
#include "common/log/log.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

InsertStmt::InsertStmt(Table *table, Value *values, int value_amount)
    : table_(table), values_(values), value_amount_(value_amount)
{}

InsertStmt::InsertStmt(Table *table, Value *values, int value_amount, std::vector<Value>* data_ptr)
    : table_(table), values_(values), value_amount_(value_amount), data_ptr_(data_ptr)
{}

RC InsertStmt::create(Db *db, InsertSqlNode &inserts, Stmt *&stmt)
{
  const char *table_name = inserts.relation_name.c_str();
  if (nullptr == db || nullptr == table_name || inserts.values.empty()) {
    LOG_WARN("invalid argument. db=%p, table_name=%p, value_num=%d",
        db, table_name, static_cast<int>(inserts.values.size()));
    return RC::INVALID_ARGUMENT;
  }

  // check whether the table exists
  Table *table = db->find_table(table_name);
  if (nullptr == table) {
    LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
    return RC::SCHEMA_TABLE_NOT_EXIST;
  }

  // check the fields number
  // Value *pvalues = inserts.values.data();
  const auto value_expr = inserts.values;
  const int value_num = static_cast<int>(inserts.values.size());
  std::vector<Value>* fvalues = new std::vector<Value>;
  fvalues->reserve(value_num);

  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num() - table_meta.sys_field_num() - table_meta.extra_field_num();
  if (field_num != value_num) {
    LOG_WARN("schema mismatch. value num=%d, field num in schema=%d", value_num, field_num);
    return RC::SCHEMA_FIELD_MISSING;
  }

  // check fields type
  const int sys_field_num = table_meta.sys_field_num();
  for (int i = 0; i < value_num; i++) {
    const FieldMeta *field_meta = table_meta.field(i + sys_field_num);
    const AttrType field_type = field_meta->type();

    if (value_expr[i]->type != PExpType::UNARY && value_expr[i]->type != PExpType::ARITHMETIC) {
      LOG_ERROR("insert stmt: %s", strrc(RC::SQL_SYNTAX));
      return RC::SQL_SYNTAX;
    }
    Value value;
    // 只有负数才会走这个逻辑
    if (value_expr[i]->type == PExpType::ARITHMETIC) {
      assert(value_expr[i]->aexp->left != nullptr);
      assert(value_expr[i]->aexp->left->uexp != nullptr);
      assert(value_expr[i]->aexp->type == PArithmeticType::NEGATIVE);
      const auto& v = value_expr[i]->aexp->left->uexp->value;
      if (v.attr_type() == AttrType::INTS) {
        value.set_int(-v.get_int());
      } else if (v.attr_type() == AttrType::FLOATS) {
        value.set_int(-v.get_float());
      } else {
        assert(0);
      }
    } else {
      if (value_expr[i]->uexp->is_attr) {
        LOG_ERROR("insert stmt: %s", strrc(RC::SQL_SYNTAX));
        return RC::SQL_SYNTAX;
      }
      value = value_expr[i]->uexp->value;
    }
    const AttrType value_type = value.attr_type();
    if (field_type != value_type) {
      // 因为插入操作的词法解析无法判断字符串是TEXTS还是CHARS
      // 目前可能会出现值 TEXTS 类型而字段是 CHARS 类型
      if (field_type == AttrType::TEXTS && value_type == AttrType::CHARS) {
        value.set_type(AttrType::TEXTS);
      }
      // 如果值为 NULL, 判断该字段是否设置了 NOT NULL
      else if (value_type == AttrType::NULLS) {
        if (field_meta->is_not_null()) {
          LOG_WARN("value can not be null. table=%s, field=%s, field type=%d, value_type=%d",
          table_name, field_meta->name(), field_type, value_type);
          return RC::SCHEMA_FIELD_TYPE_MISMATCH;
        }
      } else {
        LOG_WARN("field type mismatch. table=%s, field=%s, field type=%d, value_type=%d",
          table_name, field_meta->name(), field_type, value_type);
        return RC::SCHEMA_FIELD_TYPE_MISMATCH;
      }
    }
    fvalues->emplace_back(value);
  }

  // everything alright
  stmt = new InsertStmt(table, fvalues->data(), value_num, fvalues);
  return RC::SUCCESS;
}
