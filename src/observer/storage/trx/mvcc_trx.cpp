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
// Created by Wangyunlai on 2023/04/24.
//

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include "storage/default/default_handler.h"
#include "storage/record/record.h"
#include "storage/trx/mvcc_trx.h"
#include "storage/field/field.h"
#include "storage/clog/clog.h"
#include "storage/db/db.h"
#include "storage/clog/clog.h"

#include "common/global_context.h"

using namespace std;

MvccTrxKit::~MvccTrxKit()
{
  vector<Trx *> tmp_trxes;
  tmp_trxes.swap(trxes_);

  for (Trx *trx : tmp_trxes) {
    delete trx;
  }
}

RC MvccTrxKit::init()
{
  fields_ = vector<FieldMeta>{
      FieldMeta("__trx_xid_begin",
          AttrType::INTS,
          0 /*attr_offset*/,
          4 /*attr_len*/,
          0 /*attr_id=*/,
          false /*visible*/,
          false /*is_not_null*/),
      FieldMeta("__trx_xid_end",
          AttrType::INTS,
          4 /*attr_offset*/,
          4 /*attr_len*/,
          1 /*attr_id=*/,
          false /*visible*/,
          false /*is_not_null*/),
      FieldMeta("__trx_xid_pointer",
          AttrType::INTS,
          8 /*attr_offset*/,
          8 /*attr_len*/,
          2 /*attr_id=*/,
          false /*visible*/,
          false /*is_not_null*/),  // 指向newer的record/tuple, 用于rollback
  };

  LOG_INFO("init mvcc trx kit done.");
  return RC::SUCCESS;
}

const vector<FieldMeta> *MvccTrxKit::trx_fields() const { return &fields_; }

int32_t MvccTrxKit::next_trx_id() { return ++current_trx_id_; }

int32_t MvccTrxKit::max_trx_id() const { return numeric_limits<int32_t>::max(); }

Trx *MvccTrxKit::create_trx(CLogManager *log_manager)
{
  Trx *trx = new MvccTrx(*this, log_manager);
  if (trx != nullptr) {
    lock_.lock();
    trxes_.push_back(trx);
    lock_.unlock();
  }
  return trx;
}

Trx *MvccTrxKit::create_trx(int32_t trx_id)
{
  Trx *trx = new MvccTrx(*this, trx_id);
  if (trx != nullptr) {
    lock_.lock();
    trxes_.push_back(trx);
    if (current_trx_id_ < trx_id) {
      current_trx_id_ = trx_id;
    }
    lock_.unlock();
  }
  return trx;
}

void MvccTrxKit::destroy_trx(Trx *trx)
{
  lock_.lock();
  for (auto iter = trxes_.begin(), itend = trxes_.end(); iter != itend; ++iter) {
    if (*iter == trx) {
      trxes_.erase(iter);
      break;
    }
  }
  lock_.unlock();

  delete trx;
}

Trx *MvccTrxKit::find_trx(int32_t trx_id)
{
  lock_.lock();
  for (Trx *trx : trxes_) {
    if (trx->id() == trx_id) {
      lock_.unlock();
      return trx;
    }
  }
  lock_.unlock();

  return nullptr;
}

void MvccTrxKit::all_trxes(std::vector<Trx *> &trxes)
{
  lock_.lock();
  trxes = trxes_;
  lock_.unlock();
}

////////////////////////////////////////////////////////////////////////////////

MvccTrx::MvccTrx(MvccTrxKit &kit, CLogManager *log_manager) : trx_kit_(kit), log_manager_(log_manager) {}

MvccTrx::MvccTrx(MvccTrxKit &kit, int32_t trx_id) : trx_kit_(kit), trx_id_(trx_id)
{
  started_ = true;
  recovering_ = true;
}

MvccTrx::~MvccTrx() {}

RC MvccTrx::insert_record(Table *table, Record &record)
{
  Field begin_field;
  Field end_field;
  Field pointer_field;
  trx_fields(table, begin_field, end_field, pointer_field);

  begin_field.set_int(record, -trx_id_);
  end_field.set_int(record, trx_kit_.max_trx_id());

  RC rc = table->insert_record(record);

  // 注意: 插入record进table中, 才能改变相应的系统字段. 因为insert只关心用户字段
  Record table_record;
  table->get_record(record.rid(), table_record);

  // 用 null_rid 来表示最新的record
  RID null_rid{0, 0};
  char *new_record_pointer = table_record.data() + pointer_field.meta()->offset();
  memcpy(new_record_pointer, &null_rid, sizeof(RID));

  if (rc != RC::SUCCESS) {
    LOG_WARN("failed to insert record into table. rc=%s", strrc(rc));
    return rc;
  }

  rc = log_manager_->append_log(
      CLogType::INSERT, trx_id_, table->table_id(), record.rid(), record.len(), 0 /*offset*/, record.data());
  ASSERT(rc == RC::SUCCESS,
      "failed to append insert record log. trx id=%d, table id=%d, rid=%s, record len=%d, rc=%s",
      trx_id_,
      table->table_id(),
      record.rid().to_string().c_str(),
      record.len(),
      strrc(rc));

  pair<OperationSet::iterator, bool> ret = operations_.insert(Operation(Operation::Type::INSERT, table, record.rid()));
  if (!ret.second) {
    rc = RC::INTERNAL;
    LOG_WARN("failed to insert operation(insertion) into operation set: duplicate");
  }
  return rc;
}

RC MvccTrx::delete_record(Table *table, Record &record)
{
  Field begin_field;
  Field end_field;
  Field pointer_field;
  trx_fields(table, begin_field, end_field, pointer_field);

  [[maybe_unused]] int32_t end_xid = end_field.get_int(record);
  /// 在删除之前，第一次获取record时，就已经对record做了对应的检查，并且保证不会有其它的事务来访问这条数据
  ASSERT(end_xid > 0,
      "concurrency conflit: other transaction is updating this record. end_xid=%d, current trx id=%d, rid=%s",
      end_xid,
      trx_id_,
      record.rid().to_string().c_str());
  if (end_xid != trx_kit_.max_trx_id()) {
    // 当前不是多版本数据中的最新记录，不需要删除
    return RC::SUCCESS;
  }

  end_field.set_int(record, -trx_id_);
  RC rc = log_manager_->append_log(CLogType::DELETE, trx_id_, table->table_id(), record.rid(), 0, 0, nullptr);
  ASSERT(rc == RC::SUCCESS,
      "failed to append delete record log. trx id=%d, table id=%d, rid=%s, record len=%d, rc=%s",
      trx_id_,
      table->table_id(),
      record.rid().to_string().c_str(),
      record.len(),
      strrc(rc));

  operations_.insert(Operation(Operation::Type::DELETE, table, record.rid()));

  return RC::SUCCESS;
}

RC MvccTrx::update_record(Table *table, Record &old_record, Record &new_record)
{
  // TODO(oldcb): CLOG
  operations_.insert(Operation(Operation::Type::UPDATE, table, old_record.rid()));

  // 0. 找到time_travel_table
  // 采用TIME-TRAVEL-STORAGE方式, 将多版本的tuple存储在两张表中。
  // 表里在事务 sys_field 增加 rid 字段, newest to oldest
  // 将更新链的record存放在全局default_handler的opened_dbs_的time_travel_table中
  // Db *time_travel_db = GCTX.handler_->find_db("time_travel_db");
  Table *time_travel_table = GCTX.handler_->find_table("time_travel_db", table->name());
  if (time_travel_table == nullptr) {
    const TableMeta &table_meta = table->table_meta();
    const std::vector<FieldMeta> *field_metas = table_meta.field_metas();
    int attribute_count = table_meta.field_num() - table_meta.extra_field_num() - table_meta.sys_field_num();
    AttrInfoSqlNode attributes[attribute_count];
    for (int i = table_meta.sys_field_num(), j = 0; i < table_meta.field_num() - table_meta.extra_field_num();) {
      attributes[j].type = (*field_metas)[i].type();
      attributes[j].is_not_null = (*field_metas)[i].is_not_null();
      attributes[j].length = (*field_metas)[i].len();
      attributes[j].name = (*field_metas)[i].name();
      ++j;
      ++i;
    }
    GCTX.handler_->create_table("time_travel_db", table->name(), attribute_count, attributes);
    time_travel_table = GCTX.handler_->find_table("time_travel_db", table->name());
    assert(time_travel_table != nullptr);
  }

  Field begin_field;
  Field end_field;
  Field pointer_field;
  trx_fields(table, begin_field, end_field, pointer_field);

  // 1. 设置删除标识
  int32_t end_xid = end_field.get_int(old_record);
  ASSERT(end_xid > 0,
      "concurrency conflit: other transaction is updating this record. end_xid=%d, current trx id=%d, rid=%s",
      end_xid,
      trx_id_,
      record.rid().to_string().c_str());
  if (end_xid != trx_kit_.max_trx_id()) {
    // 当前不是多版本数据中的最新记录，不需要删除
    return RC::SUCCESS;
  }

  end_field.set_int(old_record, -trx_id_);

  // 2. 将旧record放入 time-travel table 中， 设置NTO的链表
  RID rid = old_record.rid();                    // 在time-travel table中最新的record的rid
  RID null_rid{0, 0};                            // 用 null_rid 来表示最旧的record
  time_travel_table->insert_record(old_record);  // 插入后, 此时old_record的rid改变成在time-travel table的rid
  RID travel_old_record_rid = old_record.rid();
  Record travel_old_record;                      // old_record在travel table中的拷贝
  time_travel_table->get_record(travel_old_record_rid, travel_old_record);

  // 若存在在 time-travel table 的record
  if (rid != null_rid) {
    // old_record 指向 time-travel table第二新的record
    char *pointer = travel_old_record.data() + pointer_field.meta()->offset();
    memcpy(pointer, &rid, sizeof(RID));
  }
  // 否则设置null_rid, 直接插入于time-travel table
  else {
    char *pointer = travel_old_record.data() + pointer_field.meta()->offset();
    memcpy(pointer, &null_rid, sizeof(RID));
  }

  // 3. 设置插入标识
  begin_field.set_int(new_record, -trx_id_);
  end_field.set_int(new_record, trx_kit_.max_trx_id());
  // 指向在time-travel table的old_record
  char *pointer = new_record.data() + pointer_field.meta()->offset();
  memcpy(pointer, &travel_old_record_rid, sizeof(RID));

  // 4. 将新record复制到old_record原位(注意: 不能直接删除+插入, 否则table_scan会再次扫到新record)
  // TODO(oldcb): MVCC的索引INDEX POINTER
  assert(old_record.owner() == false);
  memcpy(old_record.data(),
      new_record.data(),
      table->table_meta().record_size());  // old_record的data还是在原表中的, 只是rid改变了

  return RC::SUCCESS;
}

RC MvccTrx::visit_record(Table *table, Record &record, bool readonly)
{

  auto visit_strategy = [this, readonly](int32_t begin_xid, int32_t end_xid) -> RC {
    RC rc = RC::SUCCESS;
    // 1. 这只会发生在事务
    // 1.1 插入record且提交
    // 1.2 插入并删除record且提交
    if (begin_xid > 0 && end_xid > 0) {
      if (trx_id_ >= begin_xid && trx_id_ <= end_xid) {
        if (readonly) {
          rc = RC::SUCCESS;
        } else {
          if (end_xid != trx_kit_.max_trx_id()) {
            // 对于一个已经被删除且提交的record, 不能对此进行修改
            rc = RC::LOCKED_CONCURRENCY_CONFLICT;
          } else {
            // 对于一个刚插入且提交的record, 可以继续修改
            rc = RC::SUCCESS;
          }
        }
      } else {
        rc = RC::RECORD_INVISIBLE;
      }
    }
    // 2. 这只会发生在事务
    // 2.1 刚插入并同时删除这个record且未提交
    // 2.2 或更新多次未提交
    else if (begin_xid < 0 && end_xid < 0) {
      assert(begin_xid == end_xid);
      rc = RC::RECORD_INVISIBLE;
    }
    // 3. 这只会发生在事务
    // 3.1 刚插入record且未提交
    else if (begin_xid < 0 && end_xid > 0) {
      if (-begin_xid == trx_id_) {
        rc = RC::SUCCESS;
      } else {
        rc = RC::RECORD_INVISIBLE;
      }
    }
    // 4. 这只会发生在事务
    // 4.1 对已提交插入的record, 但正在删除未提交
    else if (end_xid < 0 && begin_xid > 0) {
      if (readonly) {
        // 如果 -end_xid 就是当前事务的事务号，说明是当前事务删除的
        if (-end_xid == trx_id_) {
          rc = RC::RECORD_INVISIBLE;
        }
        // 如果是其他事务号
        else {
          if (trx_id_ > begin_xid) {
            rc = RC::SUCCESS;
          }
        }
      } else {
        // 如果当前想要修改此条数据，并且不是当前事务删除的，简单的报错
        // 这是事务并发处理的一种方式，非常简单粗暴。其它的并发处理方法，可以等待，或者让客户端重试
        // 或者等事务结束后，再检测修改的数据是否有冲突
        rc = (-end_xid != trx_id_) ? RC::LOCKED_CONCURRENCY_CONFLICT : RC::RECORD_INVISIBLE;
      }
    }

    return rc;
  };

  Field begin_field;
  Field end_field;
  Field pointer_field;
  trx_fields(table, begin_field, end_field, pointer_field);

  int32_t begin_xid = begin_field.get_int(record);
  int32_t end_xid = end_field.get_int(record);
  RID pointer_xid;

  char *record_pointer = record.data() + pointer_field.meta()->offset();
  memcpy(&pointer_xid, record_pointer, sizeof(RID));

  RC rc = RC::RECORD_INVISIBLE;

  // 若pointer_xid非{0,0}, 说明存在一条更新链(NTO)
  // 注意: 最下层的算子只能是table scan
  // TODO(oldcb): 索引MVCC
  if (pointer_xid.page_num != 0 || pointer_xid.slot_num != 0) {
    // 若只读, 应该尝试遍历更新链找到可见的record
    if (readonly) {
      rc = visit_strategy(begin_xid, end_xid);
      // 尝试在travel_table中寻找
      if (rc != RC::SUCCESS) {
        Record travel_record;
        // 找到time_travel_table
        Table *time_travel_table = GCTX.handler_->find_table("time_travel_db", table->name());
        assert(time_travel_table != nullptr);
        time_travel_table->get_record(pointer_xid, travel_record);
        while (pointer_xid.page_num != 0 || pointer_xid.slot_num != 0) {
          begin_xid = begin_field.get_int(travel_record);
          end_xid = end_field.get_int(travel_record);
          rc = visit_strategy(begin_xid, end_xid);
          if (rc == RC::SUCCESS) {
            memcpy(record.data(), travel_record.data(), table->table_meta().record_size());
            break;
          }
          char *pointer = travel_record.data() + pointer_field.meta()->offset();
          memcpy(&pointer_xid, pointer, sizeof(RID));
          time_travel_table->get_record(pointer_xid, travel_record);
        }
        if (rc != RC::SUCCESS) {
          // 不要忘了对链表最后的record也进行visit
          begin_xid = begin_field.get_int(travel_record);
          end_xid = end_field.get_int(travel_record);
          rc = visit_strategy(begin_xid, end_xid);
          if (rc == RC::SUCCESS) {
            memcpy(record.data(), travel_record.data(), table->table_meta().record_size());
          }
        }
      }
    }
    // 若非只读, 除非是做更改的那个事务, 其他事务应该阻塞或回滚
    else {
      assert(begin_xid < 0 && end_xid > 0);
      if (-begin_xid == trx_id_) {
        rc = RC::SUCCESS;
      } else {
        rc = RC::RECORD_INVISIBLE;
      }
    }
  } else {
    rc = visit_strategy(begin_xid, end_xid);
  }

  return rc;
}
/**
 * @brief 获取指定表上的事务使用的字段
 *
 * @param table 指定的表
 * @param begin_xid_field 返回处理begin_xid的字段
 * @param end_xid_field   返回处理end_xid的字段
 */
void MvccTrx::trx_fields(Table *table, Field &begin_xid_field, Field &end_xid_field, Field &pointer_xid_field) const
{
  const TableMeta &table_meta = table->table_meta();
  const std::pair<const FieldMeta *, int> trx_fields = table_meta.trx_fields();
  ASSERT(trx_fields.second >= 2, "invalid trx fields number. %d", trx_fields.second);

  begin_xid_field.set_table(table);
  begin_xid_field.set_field(&trx_fields.first[0]);
  end_xid_field.set_table(table);
  end_xid_field.set_field(&trx_fields.first[1]);
  pointer_xid_field.set_table(table);
  pointer_xid_field.set_field(&trx_fields.first[2]);
}

RC MvccTrx::start_if_need()
{
  if (!started_) {
    ASSERT(operations_.empty(), "try to start a new trx while operations is not empty");
    trx_id_ = trx_kit_.next_trx_id();
    LOG_DEBUG("current thread change to new trx with %d", trx_id_);
    RC rc = log_manager_->begin_trx(trx_id_);
    ASSERT(rc == RC::SUCCESS, "failed to append log to clog. rc=%s", strrc(rc));
    started_ = true;
  }
  return RC::SUCCESS;
}

RC MvccTrx::commit()
{
  int32_t commit_id = trx_kit_.next_trx_id();
  return commit_with_trx_id(commit_id);
}

RC MvccTrx::commit_with_trx_id(int32_t commit_xid)
{
  // TODO 这里存在一个很大的问题，不能让其他事务一次性看到当前事务更新到的数据或同时看不到
  RC rc = RC::SUCCESS;
  started_ = false;

  for (const Operation &operation : operations_) {
    switch (operation.type()) {
      case Operation::Type::INSERT: {
        RID rid(operation.page_num(), operation.slot_num());
        Table *table = operation.table();
        Field begin_xid_field, end_xid_field, pointer_xid_field;
        trx_fields(table, begin_xid_field, end_xid_field, pointer_xid_field);

        auto record_updater = [this, &begin_xid_field, commit_xid](Record &record) {
          LOG_DEBUG("before commit insert record. trx id=%d, begin xid=%d, commit xid=%d, lbt=%s",
                    trx_id_, begin_xid_field.get_int(record), commit_xid, lbt());
          ASSERT(begin_xid_field.get_int(record) == -this->trx_id_,
              "got an invalid record while committing. begin xid=%d, this trx id=%d",
              begin_xid_field.get_int(record),
              trx_id_);

          begin_xid_field.set_int(record, commit_xid);
        };

        rc = operation.table()->visit_record(rid, false /*readonly*/, record_updater);
        ASSERT(rc == RC::SUCCESS,
            "failed to get record while committing. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));
      } break;

      case Operation::Type::DELETE: {
        Table *table = operation.table();
        RID rid(operation.page_num(), operation.slot_num());

        Field begin_xid_field, end_xid_field, pointer_xid_field;
        trx_fields(table, begin_xid_field, end_xid_field, pointer_xid_field);

        auto record_updater = [this, &end_xid_field, commit_xid](Record &record) {
          (void)this;
          ASSERT(end_xid_field.get_int(record) == -trx_id_,
              "got an invalid record while committing. end xid=%d, this trx id=%d",
              end_xid_field.get_int(record),
              trx_id_);

          end_xid_field.set_int(record, commit_xid);
        };

        rc = operation.table()->visit_record(rid, false /*readonly*/, record_updater);
        ASSERT(rc == RC::SUCCESS,
            "failed to get record while committing. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));
      } break;

      case Operation::Type::UPDATE: {
        Table *table = operation.table();
        RID rid(operation.page_num(), operation.slot_num());

        Field begin_xid_field, end_xid_field, pointer_xid_field;
        trx_fields(table, begin_xid_field, end_xid_field, pointer_xid_field);

        auto record_updater = [this, &begin_xid_field, &pointer_xid_field, commit_xid](Record &record) {
          LOG_DEBUG("before commit insert record. trx id=%d, begin xid=%d, commit xid=%d, lbt=%s",
                    trx_id_, begin_xid_field.get_int(record), commit_xid, lbt());
          ASSERT(begin_xid_field.get_int(record) == -this->trx_id_,
              "got an invalid record while committing. begin xid=%d, this trx id=%d",
              begin_xid_field.get_int(record),
              trx_id_);

          begin_xid_field.set_int(record, commit_xid);

          char *record_pointer = record.data() + pointer_xid_field.meta()->offset();
          memset(record_pointer, 0, sizeof(RID));  // TODO(oldcb): 应该也要把travel_time_table中的数据回收
        };

        rc = operation.table()->visit_record(rid, false /*readonly*/, record_updater);
        ASSERT(rc == RC::SUCCESS,
            "failed to get record while committing. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));
      } break;

      default: {
        ASSERT(false, "unsupported operation. type=%d", static_cast<int>(operation.type()));
      }
    }
  }

  operations_.clear();

  if (!recovering_) {
    rc = log_manager_->commit_trx(trx_id_, commit_xid);
  }
  LOG_TRACE("append trx commit log. trx id=%d, commit_xid=%d, rc=%s", trx_id_, commit_xid, strrc(rc));
  return rc;
}

RC MvccTrx::rollback()
{
  RC rc = RC::SUCCESS;
  started_ = false;

  for (const Operation &operation : operations_) {
    switch (operation.type()) {
      case Operation::Type::INSERT: {
        RID rid(operation.page_num(), operation.slot_num());
        Record record;
        Table *table = operation.table();
        // TODO 这里虽然调用get_record好像多次一举，而且看起来放在table的实现中更好，
        // 而且实际上trx应该记录下来自己曾经插入过的数据
        // 也就是不需要从table中获取这条数据，可以直接从当前内存中获取
        // 这里也可以不删除，仅仅给数据加个标识位，等垃圾回收器来收割也行
#if 0
        rc = table->get_record(rid, record);
        ASSERT(rc == RC::SUCCESS,
            "failed to get record while rollback. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));
        rc = table->delete_record(record);
#endif
        // 我们在这里不删除, 否则这会中断record链
        // TODO 垃圾回收
        Field begin_xid_field, end_xid_field, pointer_xid_field;
        trx_fields(table, begin_xid_field, end_xid_field, pointer_xid_field);

        auto record_deleter = [this, &begin_xid_field](Record &record) {
          ASSERT(begin_xid_field.get_int(record) == -trx_id_,
              "got an invalid record while rollback. end xid=%d, this trx id=%d",
              end_xid_field.get_int(record),
              trx_id_);

          begin_xid_field.set_int(record, numeric_limits<int32_t>::min());
        };

        rc = table->visit_record(rid, false /*readonly*/, record_deleter);
        ASSERT(rc == RC::SUCCESS,
            "failed to delete record while rollback. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));
      } break;

      case Operation::Type::DELETE: {
        Table *table = operation.table();
        RID rid(operation.page_num(), operation.slot_num());

        ASSERT(rc == RC::SUCCESS,
            "failed to get record while rollback. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));

        Field begin_xid_field, end_xid_field, pointer_xid_field;
        trx_fields(table, begin_xid_field, end_xid_field, pointer_xid_field);

        auto record_updater = [this, &end_xid_field](Record &record) {
          ASSERT(end_xid_field.get_int(record) == -trx_id_,
              "got an invalid record while rollback. end xid=%d, this trx id=%d",
              end_xid_field.get_int(record),
              trx_id_);

          end_xid_field.set_int(record, trx_kit_.max_trx_id());
        };

        rc = table->visit_record(rid, false /*readonly*/, record_updater);
        ASSERT(rc == RC::SUCCESS,
            "failed to get record while committing. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));
      } break;

      case Operation::Type::UPDATE: {
        Table *table = operation.table();
        RID rid(operation.page_num(), operation.slot_num());

        ASSERT(rc == RC::SUCCESS,
            "failed to get record while rollback. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));

        Field begin_xid_field, end_xid_field, pointer_xid_field;
        trx_fields(table, begin_xid_field, end_xid_field, pointer_xid_field);

        auto record_updater = [this, &begin_xid_field, &end_xid_field, &pointer_xid_field, table](Record &record) {
          ASSERT(begin_xid_field.get_int(record) == -trx_id_,
              "got an invalid record while rollback. end xid=%d, this trx id=%d",
              end_xid_field.get_int(record),
              trx_id_);

          RID pointer_xid;
          char *record_pointer = record.data() + pointer_xid_field.meta()->offset();
          memcpy(&pointer_xid, record_pointer, sizeof(RID));

          Record travel_record;

          // 找到time_travel_table
          Table *time_travel_table = GCTX.handler_->find_table("time_travel_db", table->name());
          assert(time_travel_table != nullptr);
          time_travel_table->get_record(pointer_xid, travel_record);
          while (pointer_xid.page_num != 0 || pointer_xid.slot_num != 0) {
            char *pointer = travel_record.data() + pointer_xid_field.meta()->offset();
            memcpy(&pointer_xid, pointer, sizeof(RID));
            time_travel_table->get_record(pointer_xid, travel_record);
          }
          // 恢复 end_xid_field
          end_xid_field.set_int(travel_record, trx_kit_.max_trx_id());

          // 将record复制原位, 并清零pointer rid
          assert(record.owner() == false);
          memcpy(record.data(), travel_record.data(), table->table_meta().record_size());
          char *pointer = travel_record.data() + pointer_xid_field.meta()->offset();
          memset(pointer, 0, sizeof(RID));
        };

        rc = table->visit_record(rid, false /*readonly*/, record_updater);
        ASSERT(rc == RC::SUCCESS,
            "failed to get record while committing. rid=%s, rc=%s",
            rid.to_string().c_str(),
            strrc(rc));
      } break;

      default: {
        ASSERT(false, "unsupported operation. type=%d", static_cast<int>(operation.type()));
      }
    }
  }

  operations_.clear();

  if (!recovering_) {
    rc = log_manager_->rollback_trx(trx_id_);
  }
  LOG_TRACE("append trx rollback log. trx id=%d, rc=%s", trx_id_, strrc(rc));
  return rc;
}

RC find_table(Db *db, const CLogRecord &log_record, Table *&table)
{
  switch (clog_type_from_integer(log_record.header().type_)) {
    case CLogType::INSERT:
    case CLogType::DELETE: {
      const CLogRecordData &data_record = log_record.data_record();
      table = db->find_table(data_record.table_id_);
      if (nullptr == table) {
        LOG_WARN("no such table to redo. table id=%d, log record=%s",
                 data_record.table_id_, log_record.to_string().c_str());
        return RC::SCHEMA_TABLE_NOT_EXIST;
      }
    } break;
    default: {
      // do nothing
    } break;
  }
  return RC::SUCCESS;
}

RC MvccTrx::redo(Db *db, const CLogRecord &log_record)
{
  Table *table = nullptr;
  RC rc = find_table(db, log_record, table);
  if (OB_FAIL(rc)) {
    return rc;
  }

  switch (log_record.log_type()) {
    case CLogType::INSERT: {
      const CLogRecordData &data_record = log_record.data_record();
      Record record;
      record.set_data(const_cast<char *>(data_record.data_), data_record.data_len_);
      record.set_rid(data_record.rid_);
      RC rc = table->recover_insert_record(record);
      if (OB_FAIL(rc)) {
        LOG_WARN("failed to recover insert. table=%s, log record=%s, rc=%s",
                 table->name(), log_record.to_string().c_str(), strrc(rc));
        return rc;
      }
      operations_.insert(Operation(Operation::Type::INSERT, table, record.rid()));
    } break;

    case CLogType::DELETE: {
      const CLogRecordData &data_record = log_record.data_record();
      Field begin_field;
      Field end_field;
      Field pointer_field;
      trx_fields(table, begin_field, end_field, pointer_field);

      auto record_updater = [this, &end_field](Record &record) {
        (void)this;
        ASSERT(end_field.get_int(record) == trx_kit_.max_trx_id(),
            "got an invalid record while committing. end xid=%d, this trx id=%d",
            end_field.get_int(record),
            trx_id_);

        end_field.set_int(record, -trx_id_);
      };

      RC rc = table->visit_record(data_record.rid_, false /*readonly*/, record_updater);
      ASSERT(rc == RC::SUCCESS,
          "failed to get record while committing. rid=%s, rc=%s",
          data_record.rid_.to_string().c_str(),
          strrc(rc));

      operations_.insert(Operation(Operation::Type::DELETE, table, data_record.rid_));
    } break;

    case CLogType::MTR_COMMIT: {
      const CLogRecordCommitData &commit_record = log_record.commit_record();
      commit_with_trx_id(commit_record.commit_xid_);
    } break;

    case CLogType::MTR_ROLLBACK: {
      rollback();
    } break;

    default: {
      ASSERT(false, "unsupported redo log. log_record=%s", log_record.to_string().c_str());
      return RC::INTERNAL;
    } break;
  }

  return RC::SUCCESS;
}
