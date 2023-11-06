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
// Created by Wangyunlai on 2021/5/14.
//

#pragma once

#include <memory>
#include <type_traits>
#include <vector>
#include <string>

#include "common/log/log.h"
#include "sql/expr/tuple_cell.h"
#include "sql/parser/parse.h"
#include "sql/parser/parse_defs.h"
#include "sql/parser/value.h"
#include "sql/expr/expression.h"
#include "storage/buffer/disk_buffer_pool.h"
#include "storage/buffer/frame.h"
#include "storage/default/default_handler.h"
#include "storage/record/record.h"
#include "storage/record/record_manager.h"
#include "common/global_context.h"

class Table;

/**
 * @defgroup Tuple
 * @brief Tuple 元组，表示一行数据，当前返回客户端时使用
 * @details
 * tuple是一种可以嵌套的数据结构。
 * 比如select t1.a+t2.b from t1, t2;
 * 需要使用下面的结构表示：
 * @code {.cpp}
 *  Project(t1.a+t2.b)
 *        |
 *      Joined
 *      /     \
 *   Row(t1) Row(t2)
 * @endcode
 *
 */

/**
 * @brief 元组的结构，包含哪些字段(这里成为Cell)，每个字段的说明
 * @ingroup Tuple
 */
class TupleSchema
{
public:
  void append_cell(const TupleCellSpec &cell) { cells_.push_back(cell); }
  void append_cell(const char *table, const char *field) { append_cell(TupleCellSpec(table, field)); }
  void append_cell(const char *alias) { append_cell(TupleCellSpec(alias)); }
  int cell_num() const { return static_cast<int>(cells_.size()); }
  const TupleCellSpec &cell_at(int i) const { return cells_[i]; }

private:
  std::vector<TupleCellSpec> cells_;
};

/**
 * @brief 元组的抽象描述
 * @ingroup Tuple
 */
class Tuple
{
public:
  Tuple() = default;
  virtual ~Tuple() = default;

  /**
   * @brief 获取元组中的Cell的个数
   * @details 个数应该与tuple_schema一致
   */
  virtual int cell_num() const = 0;

  /**
   * @brief 获取指定位置的Cell
   *
   * @param index 位置
   * @param[out] cell  返回的Cell
   */
  virtual RC cell_at(int index, Value &cell) const = 0;

  // 根据spec, 找到对应record的位置
  virtual RC find_record(const TupleCellSpec &spec, RecordPos &rid) const = 0;
  virtual RC record_at(int index, RecordPos &rid) const = 0;

  /**
   * @brief 根据cell的描述，获取cell的值
   *
   * @param spec cell的描述
   * @param[out] cell 返回的cell
   */
  virtual RC find_cell(const TupleCellSpec &spec, Value &cell) const = 0;

  // virtual std::shared_ptr<Tuple> copy_tuple() const = 0; // 换智能指针需要修改类内成员tuple为智能指针
  virtual Tuple *copy_tuple() const = 0;

  virtual std::string to_string() const
  {
    std::string str;
    const int cell_num = this->cell_num();
    for (int i = 0; i < cell_num - 1; i++) {
      Value cell;
      cell_at(i, cell);
      str += cell.to_string();
      str += ", ";
    }

    if (cell_num > 0) {
      Value cell;
      cell_at(cell_num - 1, cell);
      str += cell.to_string();
    }
    return str;
  }

protected:
  bool is_copy_ = false;  // 如果是copy的对象, 需要释放多余的资源(如new出来的record等)
};

/**
 * @brief 一行数据的元组
 * @ingroup Tuple
 * @details 直接就是获取表中的一条记录
 */
class RowTuple : public Tuple
{
public:
  RowTuple() = default;

  virtual ~RowTuple()
  {
    if (is_copy_) {
      delete record_;
    }

    for (FieldExpr *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }

  void set_record(Record *record)
  {
    const FieldMeta *null_field = speces_.back()->field().meta();

    this->bitmap_.init(record->data() + null_field->offset(), null_field->len());
    this->record_ = record;
  }

  // 默认fields是来自TableMeta, 包含了系统字段+用户字段+NULL字段
  void set_schema(const Table *table, const std::vector<FieldMeta> *fields)
  {
    table_ = table;
    this->speces_.reserve(fields->size());
    // 我们应该去除开头的系统字段
    int field_num = table->table_meta().field_num();
    int sys_field_num = table->table_meta().sys_field_num();
    for (int i = sys_field_num; i < field_num; ++i) {
      speces_.push_back(new FieldExpr(
          table, &(*fields)[i]));  // 这里field传的是指针, tuple里的speces的field一直指向的是tablemeta里的资源
    }
  }
  void set_table(const Table *table) { table_ = table; }

  void set_speces(const std::vector<FieldExpr *> &speces) { speces_ = speces; }

  // 为了和tuple_schema统一, 我们这里要减去null字段, 返回的是用户字段数
  int cell_num() const override { return speces_.size() - 1; }

  // index的默认是从非系统字段从0开始
  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }

    FieldExpr *field_expr = speces_[index];
    const FieldMeta *field_meta = field_expr->field().meta();

    // 设置类型
    cell.set_type(field_meta->type());

    // 1. 判断是否是 NULL
    if (bitmap_.get_bit(index)) {
      cell.set_null();
    }
    // 2. 如果字段是文本, 需要从表中的对应的溢出页取数据
    else if (field_meta->type() == AttrType::TEXTS) {
      Frame *frame = nullptr;
      std::string texts;  // 文本内容

      PageNum page_num;  // 溢出页号
      int record_offset{0};
      memcpy(&page_num,
          this->record_->data() + field_meta->offset() + record_offset,
          sizeof(PageNum));  // 溢出页号保存在record中
      while (page_num != 0) {
        DiskBufferPool *data_buffer_pool = const_cast<DiskBufferPool *>(table_->data_buffer_pool());
        data_buffer_pool->get_this_page(page_num, &frame);

        std::string text;
        frame->read_latch();
        text.assign((char *)frame->data() + sizeof(PageHeader));
        frame->unpin();
        frame->read_unlatch();
        texts += text;

        record_offset += sizeof(PageNum);
        memcpy(&page_num, this->record_->data() + field_meta->offset() + record_offset, sizeof(PageNum));
      }
      cell.set_data(texts.c_str(), 0);
    }
    // 3. 否则直接设置cell
    else {
      cell.set_data(this->record_->data() + field_meta->offset(), field_meta->len());
    }
    return RC::SUCCESS;
  }

  RC record_at(int index, RecordPos &rid) const override
  {
    rid = RecordPos(table_->table_id(), record_->rid());
    return RC::SUCCESS;
  }

  RC find_record(const TupleCellSpec &spec, RecordPos &rid) const override
  {
    const char *table_name = spec.table_name();
    const char *field_name = spec.field_name();
    if (0 != strcmp(table_name, table_->name())) {
      return RC::NOTFOUND;
    }

    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr *field_expr = speces_[i];
      const Field &field = field_expr->field();
      if (0 == strcmp(field_name, field.field_name())) {
        return record_at(i, rid);
      }
    }
    return RC::NOTFOUND;
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    const char *table_name = spec.table_name();
    const char *field_name = spec.field_name();
    if (0 != strcmp(table_name, table_->name())) {
      return RC::NOTFOUND;
    }

    for (size_t i = 0; i < speces_.size(); ++i) {
      const FieldExpr *field_expr = speces_[i];
      const Field &field = field_expr->field();
      if (0 == strcmp(field_name, field.field_name())) {
        return cell_at(i, cell);
      }
    }
    return RC::NOTFOUND;
  }

#if 0
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      LOG_WARN("invalid argument. index=%d", index);
      return RC::INVALID_ARGUMENT;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
#endif

  Record &record() { return *record_; }

  const Record &record() const { return *record_; }

  const common::Bitmap &bitmap() const { return bitmap_; }

  Tuple *copy_tuple() const override
  {
    // record
    RowTuple *tuple = new RowTuple();
    Record *record = new Record(*record_);

    // speces
    std::vector<FieldExpr *> speces;

    for (const FieldExpr *spece_ : speces_) {
      FieldExpr *field_expr = new FieldExpr(spece_->field());
      speces.push_back(field_expr);
    }

    tuple->set_table(table_);
    tuple->set_speces(speces);  // 必须先设置speces, 在设置 record, 因为需要speces中的元素来设置null相关的bitmap
    tuple->set_record(record);

    tuple->is_copy_ = true;

    return tuple;
  }

private:
  mutable common::Bitmap bitmap_;
  Record *record_ = nullptr;
  const Table *table_ = nullptr;
  // speces指的是用户字段+null字段
  std::vector<FieldExpr *> speces_;
};

/**
 * @brief 从一行数据中，选择部分字段组成的元组，也就是投影操作
 * @ingroup Tuple
 * @details 一般在select语句中使用。
 * 投影也可以是很复杂的操作，比如某些字段需要做类型转换、重命名、表达式运算、函数计算等。
 * 当前的实现是比较简单的，只是选择部分字段，不做任何其他操作。
 */
class ProjectTuple : public Tuple
{
public:
  ProjectTuple() = default;
  virtual ~ProjectTuple()
  {
    if (is_copy_) {
      delete tuple_;
    }

    for (TupleCellSpec *spec : speces_) {
      delete spec;
    }
    speces_.clear();
  }

  void set_tuple(Tuple *tuple) { this->tuple_ = tuple; }
  void set_speces(const std::vector<TupleCellSpec *> &speces) { this->speces_ = speces; }

  void add_cell_spec(TupleCellSpec *spec) { speces_.push_back(spec); }
  int cell_num() const override { return speces_.size(); }

  RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::INTERNAL;
    }
    if (tuple_ == nullptr) {
      return RC::INTERNAL;
    }

    const TupleCellSpec *spec = speces_[index];
    return tuple_->find_cell(*spec, cell);
  }

  RC find_cell(const TupleCellSpec &spec, Value &cell) const override { return tuple_->find_cell(spec, cell); }

  RC record_at(int index, RecordPos &rid) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::INTERNAL;
    }
    if (tuple_ == nullptr) {
      return RC::INTERNAL;
    }

    const TupleCellSpec *spec = speces_[index];
    return tuple_->find_record(*spec, rid);
  }

  RC find_record(const TupleCellSpec &spec, RecordPos &rid) const override { return tuple_->find_record(spec, rid); }

  Tuple *copy_tuple() const override
  {
    ProjectTuple *tuple = new ProjectTuple();

    // speces
    std::vector<TupleCellSpec *> speces;
    for (const TupleCellSpec *spece_ : speces_) {
      TupleCellSpec *field_expr = new TupleCellSpec(spece_->table_name(), spece_->field_name(), spece_->alias());
      speces.push_back(field_expr);
    }
    tuple->set_speces(speces);

    // tuple
    Tuple *tuple_member = this->tuple_->copy_tuple();
    tuple->set_tuple(tuple_member);

    tuple->is_copy_ = true;

    return tuple;
  }

#if 0
  RC cell_spec_at(int index, const TupleCellSpec *&spec) const override
  {
    if (index < 0 || index >= static_cast<int>(speces_.size())) {
      return RC::NOTFOUND;
    }
    spec = speces_[index];
    return RC::SUCCESS;
  }
#endif
private:
  std::vector<TupleCellSpec *> speces_;
  Tuple *tuple_ = nullptr;
};

/**
 * @description: 新定义的一个tuple, 支持一些别的操作
 * @return {*}
 */
class ExpressionTuple : public Tuple
{
public:
  ExpressionTuple() {}

  virtual ~ExpressionTuple() {}

  int cell_num() const override { return expressions_->size(); }

  void set_expressions(std::vector<std::unique_ptr<Expression>> *expressions) { expressions_ = expressions; }
  void set_tuple(Tuple *tuple) { tuple_ = tuple; }

  RC cell_at(int index, Value &cell) const override
  {
    if (!expressions_  || index < 0 || index >= static_cast<int>(expressions_->size())) {
      LOG_ERROR("calc physical operator error");
      return RC::INTERNAL;
    }
    Expression *expr = (*expressions_)[index].get();
    RC rc = expr->get_value(*tuple_, cell);
    return rc;
  }

  // 对于视图, 在表中存放了他的表达式的名字
  // 一般自上到下会遇到expression的find_cell的情况都是视图这种算子串联导致的
  RC find_cell(const TupleCellSpec &spec, Value &cell) const override
  {
    // 直接从全局找视图表, 对比表头
    Table *view_table = GCTX.handler_->find_table("sys", spec.table_name());
    assert(view_table != nullptr);

    TupleSchema *schema = view_table->schema();

    for (int i = 0; i < schema->cell_num(); ++i) {
      std::string view_alias = view_table->schema()->cell_at(i).alias();
      std::string table_alias = spec.field_name();
      std::string view_name;
      std::string table_name;
      if (!view_alias.empty()) {
        size_t dotPos = view_alias.find('.');
        if (dotPos != std::string::npos) {
          view_name = view_alias.substr(dotPos + 1);
        } else {
          view_name = view_alias;
        }
      }
      if (!table_alias.empty()) {
        size_t dotPos = table_alias.find('.');
        if (dotPos != std::string::npos) {
          table_name = table_alias.substr(dotPos + 1);
        } else {
          table_name = table_alias;
        }
      }
      if (0 == table_name.compare(view_name)) {
        return cell_at(i, cell);
      }
    }

    return RC::NOTFOUND;
  }

  RC record_at(int index, RecordPos &rid) const override
  {
    if (!expressions_ || !tuple_ || index < 0 || index >= static_cast<int>(expressions_->size())) {
      return RC::INTERNAL;
    }
    Expression *expr = (*expressions_)[index].get();
    RC rc = expr->get_record(*tuple_, rid);
    return rc;
  }

  RC find_record(const TupleCellSpec &spec, RecordPos &rid) const override
  {  // 直接从全局找视图表, 对比表头
    Table *view_table = GCTX.handler_->find_table("sys", spec.table_name());
    assert(view_table != nullptr);

    TupleSchema *schema = view_table->schema();

    for (int i = 0; i < schema->cell_num(); ++i) {
      std::string view_alias = view_table->schema()->cell_at(i).alias();
      std::string table_alias = spec.field_name();
      std::string view_name;
      std::string table_name;
      if (!view_alias.empty()) {
        size_t dotPos = view_alias.find('.');
        if (dotPos != std::string::npos) {
          view_name = view_alias.substr(dotPos + 1);
        } else {
          view_name = view_alias;
        }
      }
      if (!table_alias.empty()) {
        size_t dotPos = table_alias.find('.');
        if (dotPos != std::string::npos) {
          table_name = table_alias.substr(dotPos + 1);
        } else {
          table_name = table_alias;
        }
      }
      if (0 == table_name.compare(view_name)) {
        return record_at(i, rid);
      }
    }

    return RC::NOTFOUND;
  }

  // TODO
  // 不能 Copy 应该, 因为成员对象是引用
  Tuple *copy_tuple() const override
  {
    LOG_WARN("invalid");
    return nullptr;
  }

private:
  std::vector<std::unique_ptr<Expression>> *expressions_;  // not own this;
  Tuple *tuple_;                                           // not own this;
};

/**
 * @brief 一些常量值组成的Tuple
 * @ingroup Tuple
 */
class ValueListTuple : public Tuple
{
public:
  ValueListTuple() = default;
  virtual ~ValueListTuple() = default;

  void set_cells(const std::vector<Value> &cells) { cells_ = cells; }

  virtual int cell_num() const override { return static_cast<int>(cells_.size()); }

  virtual RC cell_at(int index, Value &cell) const override
  {
    if (index < 0 || index >= cell_num()) {
      return RC::NOTFOUND;
    }

    cell = cells_[index];
    return RC::SUCCESS;
  }

  Tuple *copy_tuple() const override
  {
    ValueListTuple *tuple = new ValueListTuple();
    tuple->set_cells(cells_);

    tuple->is_copy_ = true;  // do nothing here

    return tuple;
  }

  virtual RC find_cell(const TupleCellSpec &spec, Value &cell) const override { return RC::INTERNAL; }

  RC record_at(int index, RecordPos &rid) const override { return RC::NOTFOUND; }

  RC find_record(const TupleCellSpec &spec, RecordPos &rid) const override { return RC::NOTFOUND; }

private:
  std::vector<Value> cells_;
};

/**
 * @brief 将两个tuple合并为一个tuple
 * @ingroup Tuple
 * @details 在join算子中使用
 */
class JoinedTuple : public Tuple
{
public:
  JoinedTuple() = default;
  virtual ~JoinedTuple()
  {
    if (is_copy_) {
      delete left_;
      delete right_;
    }
  }

  void set_left(Tuple *left) { left_ = left; }
  void set_right(Tuple *right) { right_ = right; }

  int cell_num() const override { return left_->cell_num() + right_->cell_num(); }

  RC cell_at(int index, Value &value) const override
  {
    const int left_cell_num = left_->cell_num();
    if (index > 0 && index < left_cell_num) {
      return left_->cell_at(index, value);
    }

    if (index >= left_cell_num && index < left_cell_num + right_->cell_num()) {
      return right_->cell_at(index - left_cell_num, value);
    }

    return RC::NOTFOUND;
  }

  RC find_cell(const TupleCellSpec &spec, Value &value) const override
  {
    RC rc = left_->find_cell(spec, value);
    if (rc == RC::SUCCESS || rc != RC::NOTFOUND) {
      return rc;
    }

    return right_->find_cell(spec, value);
  }

  RC record_at(int index, RecordPos &rid) const override
  {
    const int left_cell_num = left_->cell_num();
    if (index > 0 && index < left_cell_num) {
      return left_->record_at(index, rid);
    }

    if (index >= left_cell_num && index < left_cell_num + right_->cell_num()) {
      return right_->record_at(index - left_cell_num, rid);
    }

    return RC::NOTFOUND;
  }

  RC find_record(const TupleCellSpec &spec, RecordPos &rid) const override
  {
    RC rc = left_->find_record(spec, rid);
    if (rc == RC::SUCCESS || rc != RC::NOTFOUND) {
      return rc;
    }

    return right_->find_record(spec, rid);
  }

  auto &get_left() const { return left_; }
  auto &get_right() const { return right_; }

  Tuple *copy_tuple() const override
  {
    JoinedTuple *tuple = new JoinedTuple();
    tuple->set_left(left_->copy_tuple());
    tuple->set_right(right_->copy_tuple());
    tuple->is_copy_ = true;

    return tuple;
  }

private:
  Tuple *left_ = nullptr;
  Tuple *right_ = nullptr;
};

/**
 * @brief 聚合后的tuple
 * @ingroup Tuple
 * @details 在aggregation 算子中使用
 */
class AggregationTuple : public Tuple
{
public:
  /**
   * @description:
   * @param {vector<Value>} &aggregations  表示聚合后的值
   * @param {vector<Field>} &fields 这个字段没啥用
   * @return {*}
   */
  AggregationTuple(const std::vector<Value> &aggregations) : aggregations_(aggregations) {}
  virtual ~AggregationTuple() = default;

  int cell_num() const override { return static_cast<int>(aggregations_.size()); }

  RC cell_at(int index, Value &value) const override
  {
    if (index >= 0 && index < aggregations_.size()) {
      value = aggregations_[index];
      return RC::SUCCESS;
    }
    return RC::NOTFOUND;
  }

  RC find_cell(const TupleCellSpec &spec, Value &value) const override
  {
    LOG_WARN("aggr find_cell: %s", strrc(RC::NOT_IMPLEMENT));
    return RC::NOT_IMPLEMENT;
    // assert(aggregations_.size() == fields_.size());
    // for (int i = 0; i < fields_.size(); i++) {
    //   // if (field.equal(spec.)
    //   const auto &field = fields_[i];
    //   if (!strcmp(spec.table_name(), field.table_name())) {
    //     if (!strcmp(spec.field_name(), field.field_name())) {
    //       value = aggregations_[i];
    //       return RC::SUCCESS;
    //     }
    //   }
    // }
  }

  RC record_at(int index, RecordPos &rid) const override { return RC::NOTFOUND; }

  RC find_record(const TupleCellSpec &spec, RecordPos &rid) const override { return RC::NOTFOUND; }

  Tuple *copy_tuple() const override
  {
    AggregationTuple *tuple = new AggregationTuple(aggregations_);
    tuple->is_copy_ = true;  // do nothing here
    return tuple;
  }

  // aggregations 表示groupby返回一行的内容，为分组名+聚合的值
  void set_aggregations(const std::vector<Value> &aggregations)
  {
    aggregations_.clear();
    aggregations_ = aggregations;
  }

private:
  std::vector<Value> aggregations_;
  // 聚合的字段
  // std::vector<Field> fields_;
  // std::vector<std::unique_ptr<Expression>> aggr_exprs_;
};
