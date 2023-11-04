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
// Created by Wangyunlai on 2022/6/6.
//

#include "sql/stmt/select_stmt.h"
#include "sql/parser/parse_defs.h"
#include "sql/stmt/filter_stmt.h"
#include "common/log/log.h"
#include "common/lang/string.h"
#include "storage/db/db.h"
#include "storage/table/table.h"

SelectStmt::~SelectStmt()
{
  if (nullptr != filter_stmt_) {
    delete filter_stmt_;
    filter_stmt_ = nullptr;
  }

  if (join_on_stmt_ != nullptr) {
    delete join_on_stmt_;
  }

  if (having_stmt_ != nullptr) {
    delete having_stmt_;
  }
}


static void set_expression_name(Expression* expr, const std::string& table_name, 
  const std::string& fieldname, bool with_table_name) 
{
  std::string name;
  if (with_table_name) {
    name = table_name + ".";
  }
  expr->set_name(name + fieldname);
}

static void wildcard_fields(Table *table, std::vector<std::unique_ptr<Expression>> &field_metas, bool with_table_name)
{
  const TableMeta &table_meta = table->table_meta();
  
  const int field_num = table_meta.field_num() - table_meta.extra_field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    field_metas.emplace_back(new FieldExpr(Field(table, table_meta.field(i))));
    set_expression_name(field_metas.back().get(), table->name(), table_meta.field(i)->name(), with_table_name); 
  }
}

RC SelectStmt::createField(
    const std::vector<Table *> &tables, const char *table_name, const char *attr_name, Field &field)
{
  // 如果没有使用table.attr name的形式，默认使用tables[0]的表名，建立排序字段
  std::string table_name_str(table_name);
  if (table_name_str.empty()) {
    if (tables.size() != 1) {
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    table_name_str = tables[0]->name();
  }
  for (auto table : tables) {
    if (!strcmp(table->name(), table_name_str.c_str())) {
      const FieldMeta *field_meta = table->table_meta().field(attr_name);
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s", table->name(), attr_name);
        return RC::SCHEMA_FIELD_MISSING;
      }
      field = Field(table, field_meta);
      return RC::SUCCESS;
    }
  }
  return RC::SCHEMA_FIELD_NOT_EXIST;
}

RC SelectStmt::create(Db *db, const SelectSqlNode &select_sql, Stmt *&stmt)
{
  if (nullptr == db) {
    LOG_WARN("invalid argument. db is null");
    return RC::INVALID_ARGUMENT;
  }

  // collect tables in `from` statement
  std::vector<Table *> tables;
  std::unordered_map<std::string, Table *> table_map;
  for (size_t i = 0; i < select_sql.relations.size(); i++) {
    const char *table_name = select_sql.relations[i].relation_name.c_str();
    if (nullptr == table_name) {
      LOG_WARN("invalid argument. relation name is null. index=%d", i);
      return RC::INVALID_ARGUMENT;
    }

    Table *table = db->find_table(table_name);
    if (nullptr == table) {
      LOG_WARN("no such table. db=%s, table_name=%s", db->name(), table_name);
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }

    tables.push_back(table);
    table_map.insert(std::pair<std::string, Table *>(table_name, table));
  }

  // 投影表达式
  std::vector<std::unique_ptr<Expression>> projects;

  bool with_table_name = tables.size() > 1;

  // 聚合字段的个数
  int aggr_field_cnt = 0;
  for (int i = static_cast<int>(select_sql.attributes.size()) - 1; i >= 0; i--) {
    // TODO: 考虑表达式或子查询, 这里我默认拿出来的就是一个字段, 即attr_pexp是unary的
    PExpr *attr_pexp = select_sql.attributes[i];
    // 这里处理算数类型的表达式
    if (attr_pexp->type == PExpType::ARITHMETIC) {
      Expression* a_expr = nullptr;
      RC rc = Expression::create_expression(attr_pexp, table_map, a_expr);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      a_expr->set_name(attr_pexp->name);
      projects.emplace_back(a_expr);
      continue;
    } else if (attr_pexp->type == PExpType::AGGRFUNC) {   // 聚合
      Expression* ag_expr = nullptr;
      RC rc = Expression::create_expression(attr_pexp, table_map, ag_expr);
      if (rc != RC::SUCCESS) {
        return rc;
      }
      ag_expr->set_name(attr_pexp->name);
      projects.emplace_back(ag_expr);
      aggr_field_cnt++;
      continue;
    }

    // 不带聚合select的逻辑, 这边就是正常的field表达式
    const RelAttrSqlNode &relation_attr = attr_pexp->uexp->attr;
    if (attr_pexp->type != PExpType::UNARY || !attr_pexp->uexp->is_attr) {
      LOG_WARN("not implemented");
    }
    
    // select * from;
    if (common::is_blank(relation_attr.relation_name.c_str()) &&
        0 == strcmp(relation_attr.attribute_name.c_str(), "*")) { 
      for (Table *table : tables) {
        wildcard_fields(table, projects, with_table_name);
      }
    } else if (!common::is_blank(relation_attr.relation_name.c_str())) {
      const char *table_name = relation_attr.relation_name.c_str();
      const char *field_name = relation_attr.attribute_name.c_str();
      //select *.* from xxx;
      if (0 == strcmp(table_name, "*")) {
        if (0 != strcmp(field_name, "*")) {
          LOG_WARN("invalid field name while table is *. attr=%s", field_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
        for (Table *table : tables) {
          wildcard_fields(table, projects, with_table_name);
        }
      } else {
        auto iter = table_map.find(table_name);
        if (iter == table_map.end()) {
          LOG_WARN("no such table in from list: %s", table_name);
          return RC::SCHEMA_FIELD_MISSING;
        }

        Table *table = iter->second;
        //   select table.* from xxx;
        if (0 == strcmp(field_name, "*")) {
          wildcard_fields(table, projects, with_table_name);
        } else {

          //  select table.rel from xxx;
          const FieldMeta *field_meta = table->table_meta().field(field_name);
          if (nullptr == field_meta) {
            LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
            return RC::SCHEMA_FIELD_MISSING;
          }
          projects.emplace_back(new FieldExpr(Field(table, field_meta)));
          set_expression_name(projects.back().get(), table->name(), field_meta->name(), with_table_name);
        }
      }

    } else {
      // id
      if (tables.size() != 1) {
        LOG_WARN("invalid. I do not know the attr's table. attr=%s", relation_attr.attribute_name.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }

      Table *table = tables[0];
      const FieldMeta *field_meta = table->table_meta().field(relation_attr.attribute_name.c_str());
      if (nullptr == field_meta) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), relation_attr.attribute_name.c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }
      projects.emplace_back(new FieldExpr(Field(table, field_meta)));
      set_expression_name(projects.back().get(), table->name(), field_meta->name(), with_table_name);
    }
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), projects.size());

  if (aggr_field_cnt != 0 && aggr_field_cnt != static_cast<int>(select_sql.attributes.size()) &&
    select_sql.groupbys.empty()) {
    LOG_WARN("num of aggregation is invalid");
    return RC::INVALID_ARGUMENT;
  }

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }

  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db, default_table, &table_map, select_sql.conditions, filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // create join on statement
  JoinOnStmt *join_on_stmt = nullptr;
  rc = JoinOnStmt::create(db, default_table, &table_map, select_sql.join_conds, join_on_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // create orderby
  auto orderbys = std::make_unique<std::vector<OrderByUnit>>();
  orderbys->reserve(select_sql.orderbys.size());
  for (auto &order_by : select_sql.orderbys) {
    Field field;
    rc = createField(tables, order_by.attr.relation_name.c_str(), order_by.attr.attribute_name.c_str(), field);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    orderbys->emplace_back(field, order_by.sort_type);
  }

  // groupbys
  std::vector<Field> groupbys;
  for (const auto& groupby_attr: select_sql.groupbys) {
    Field field;
    RC rc = createField(tables, groupby_attr.relation_name.c_str(), groupby_attr.attribute_name.c_str(), field);
    if (rc != RC::SUCCESS) {
      LOG_WARN("groupby key error");
      return rc;
    }
    groupbys.emplace_back(field);
  }
  
  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  select_stmt->tables_.swap(tables);
  select_stmt->projects.swap(projects);
  select_stmt->join_on_stmt_ = join_on_stmt;
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->orderbys_ = std::move(orderbys);
  select_stmt->groupbys_.swap(groupbys);

  // groupby or aggregation
  if (aggr_field_cnt > 0) {
    if (select_stmt->groupbys_.empty()) {
      select_stmt->whereConditoin = WhereConditoin::AGGREGATION;
    } else {
      // 否则，就可以通过query_field拿到groupby的信息(那些field类型是聚合的)
      select_stmt->whereConditoin = WhereConditoin::GROUPBY;
      // create groupby, 这里偷懒，直接用filter_stmt
      FilterStmt *having_stmt = nullptr;
      rc = FilterStmt::create(db, default_table, &table_map, select_sql.havings, having_stmt);
      if (rc != RC::SUCCESS) {
        LOG_WARN("cannot construct filter stmt");
        return rc;
      }
      select_stmt->having_stmt_ = having_stmt;
    }
  }
 
  stmt = select_stmt;
  return RC::SUCCESS;
}
