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
}

static void wildcard_fields(Table *table, std::vector<Field> &field_metas)
{
  const TableMeta &table_meta = table->table_meta();
  const int field_num = table_meta.field_num() - table_meta.extra_field_num();
  for (int i = table_meta.sys_field_num(); i < field_num; i++) {
    field_metas.push_back(Field(table, table_meta.field(i)));
  }
}

RC SelectStmt::createField(const std::vector<Table*> &tables, const char* table_name, const char* attr_name, Field& field) {
  // 如果没有使用table.attr name的形式，默认使用tables[0]的表名，建立排序字段
  std::string table_name_str(table_name);
  if (table_name_str.empty()) {
    if (tables.size() != 1) {
      return RC::SCHEMA_TABLE_NOT_EXIST;
    }
    table_name_str = tables[0]->name();
  }
  for (auto table: tables) {
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
    const char *table_name = select_sql.relations[i].c_str();
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

  // collect query fields in `select` statement
  std::vector<Field> query_fields;

  // 聚合字段的个数
  int aggr_field_cnt = 0;
  for (int i = static_cast<int>(select_sql.attributes.size()) - 1; i >= 0; i--) {
    const RelAttrSqlNode &relation_attr = select_sql.attributes[i];
    auto aggr_type = relation_attr.aggr_type;

    // 检验聚合的合法性
    if (aggr_type != AggrFuncType::INVALID) {
      aggr_field_cnt++;
      if (relation_attr.aggregates.size() != 1) {
        return RC::SQL_SYNTAX;
      }

      if (tables.empty()) {
        return RC::SQL_SYNTAX;
      }
      if (aggr_type == AggrFuncType::COUNT && relation_attr.aggregates[i] == "*") {
        aggr_type = AggrFuncType::COUNT_STAR;
      }
      Table *table = tables[0];
      const FieldMeta *field_meta = table->table_meta().field(relation_attr.aggregates[0].c_str());
      if (nullptr == field_meta && aggr_type != AggrFuncType::COUNT_STAR) {
        LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), relation_attr.aggregates[0].c_str());
        return RC::SCHEMA_FIELD_MISSING;
      }
      query_fields.push_back(Field(table, field_meta, aggr_type));
      continue;
    }

    // 不带聚合select的逻辑
    if (common::is_blank(relation_attr.relation_name.c_str()) &&
        0 == strcmp(relation_attr.attribute_name.c_str(), "*")) {
      for (Table *table : tables) {
        wildcard_fields(table, query_fields);
      }
    } else if (!common::is_blank(relation_attr.relation_name.c_str())) {
      const char *table_name = relation_attr.relation_name.c_str();
      const char *field_name = relation_attr.attribute_name.c_str();

      if (0 == strcmp(table_name, "*")) {
        if (0 != strcmp(field_name, "*")) {
          LOG_WARN("invalid field name while table is *. attr=%s", field_name);
          return RC::SCHEMA_FIELD_MISSING;
        }
        for (Table *table : tables) {
          wildcard_fields(table, query_fields);
        }
      } else {
        auto iter = table_map.find(table_name);
        if (iter == table_map.end()) {
          LOG_WARN("no such table in from list: %s", table_name);
          return RC::SCHEMA_FIELD_MISSING;
        }

        Table *table = iter->second;
        if (0 == strcmp(field_name, "*")) {
          wildcard_fields(table, query_fields);
        } else {
          const FieldMeta *field_meta = table->table_meta().field(field_name);
          if (nullptr == field_meta) {
            LOG_WARN("no such field. field=%s.%s.%s", db->name(), table->name(), field_name);
            return RC::SCHEMA_FIELD_MISSING;
          }
          query_fields.push_back(Field(table, field_meta));
        }
      }
    } else {
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
      query_fields.push_back(Field(table, field_meta));
    }
  }

  LOG_INFO("got %d tables in from stmt and %d fields in query stmt", tables.size(), query_fields.size());

  if (aggr_field_cnt != 0 && aggr_field_cnt != static_cast<int>(select_sql.attributes.size())) {
    LOG_WARN("num of aggregation is invalid");
    return RC::INVALID_ARGUMENT;
  }

  Table *default_table = nullptr;
  if (tables.size() == 1) {
    default_table = tables[0];
  }


  // create filter statement in `where` statement
  FilterStmt *filter_stmt = nullptr;
  RC rc = FilterStmt::create(db,
      default_table,
      &table_map,
      select_sql.conditions.data(),
      static_cast<int>(select_sql.conditions.size()),
      filter_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  std::vector<std::vector<ConditionSqlNode>> conditions;

  // 把joins里面, a.id > 3 这种condition 放到filter里面
  for (int i = 0; i < select_sql.join_conds.size(); i++) {
    std::vector<ConditionSqlNode> join_on_condtions;
    for (auto condition: select_sql.join_conds[i]) {
      if (condition.comp != CompOp::EQUAL_TO) {
        // 这种先不支持
        if (condition.left_is_attr && condition.right_is_attr) {
          LOG_WARN("not support attr comp attr type");
        } else {
          FilterUnit* filter_unit = nullptr;
          rc = FilterStmt::create_filter_unit(db, default_table, &table_map, condition, filter_unit);
          if (rc != RC::SUCCESS) {
            return rc;
          }
          filter_stmt->addFilterUnit(filter_unit);
        }
      } else {
        join_on_condtions.push_back(condition);
      }
    }
    conditions.emplace_back(std::move(join_on_condtions));
  }

  // create join on statement
  JoinOnStmt *join_on_stmt = nullptr;
  rc = JoinOnStmt::create(db,
      default_table,
      &table_map,
      conditions.data(),
      static_cast<int>(conditions.size()),
      join_on_stmt);
  if (rc != RC::SUCCESS) {
    LOG_WARN("cannot construct filter stmt");
    return rc;
  }

  // create orderby 
  auto orderbys = std::make_unique<std::vector<OrderByUnit>>();
  orderbys->reserve(select_sql.orderbys.size());
  for (auto& order_by: select_sql.orderbys) {
    Field field;
    rc = createField(tables, order_by.attr.relation_name.c_str(), order_by.attr.attribute_name.c_str(), field);
    if (rc != RC::SUCCESS) {
      return rc;
    }
    orderbys->emplace_back(field, order_by.sort_type);
  }

  // everything alright
  SelectStmt *select_stmt = new SelectStmt();
  select_stmt->tables_.swap(tables);
  select_stmt->query_fields_.swap(query_fields);
  select_stmt->join_on_stmt_ = join_on_stmt;
  select_stmt->filter_stmt_ = filter_stmt;
  select_stmt->orderbys_ = std::move(orderbys);
  select_stmt->set_is_aggregation_stmt(aggr_field_cnt > 0);

  stmt = select_stmt;
  return RC::SUCCESS;
}
