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
// Created by Wangyunlai on 2021/5/12.
//

#pragma once

#include <iterator>
#include <string>
#include <vector>

#include "common/rc.h"
#include "storage/field/field_meta.h"
#include "storage/index/index_meta.h"
#include "common/lang/serializable.h"

class Table;
/**
 * @brief 表元数据
 *
 */
class TableMeta : public common::Serializable
{
public:
  TableMeta() = default;
  ~TableMeta() = default;

  TableMeta(const TableMeta &other);

  void swap(TableMeta &other) noexcept;

  RC init(int32_t table_id, const char *name, int field_num, const AttrInfoSqlNode attributes[]);

  RC add_index(const IndexMeta &index);

public:
  int32_t table_id() const { return table_id_; }
  const char *name() const;
  const FieldMeta *trx_field() const;
  const FieldMeta *field(int index) const;
  const FieldMeta *field(const char *name) const;
  const FieldMeta *find_field_by_offset(int offset) const;
  const FieldMeta *null_field() const;
  const std::vector<FieldMeta> *field_metas() const { return &fields_; }

  // 仅用于视图
  const std::vector<const Table *> &view_tables() const { return view_tables_; }
  const Table *view_table(int index) const;
  const Table *view_table(const char *name) const;

  auto trx_fields() const -> const std::pair<const FieldMeta *, int>;

  int field_num() const;        // sys field and null included
  int sys_field_num() const;    // sys field
  int extra_field_num() const;  // null field

  const IndexMeta *index(const char *name) const;
  const IndexMeta *find_index_by_field(const std::vector<std::string> &fields) const;
  const IndexMeta *index(int i) const;
  int index_num() const;

  int record_size() const;

public:
  int serialize(std::ostream &os) const override;
  int deserialize(std::istream &is) override;
  int get_serial_size() const override;
  void to_string(std::string &output) const override;
  void desc(std::ostream &os) const;

protected:
  int32_t table_id_ = -1;
  std::string name_;
  std::vector<FieldMeta> fields_;  // 最前面包含sys_fields, 最后面包含了null field
  std::vector<IndexMeta> indexes_;

  std::vector<const Table *> view_tables_;  // 对于视图来说, 要确定每个字段都来自哪张表. 因为视图是放在内存中的,
                                            // 不需要持久化.

  int record_size_ = 0;

  friend Table;
};
