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
// Created by Wangyunlai 2023/6/27
//

#pragma once

#include <stdexcept>
#include <string>

/**
 * @brief 属性的类型
 *
 */
enum AttrType
{
  UNDEFINED,
  NULLS,     ///< NULL类型
  DATES,     ///< 日期类型
  CHARS,     ///< 字符串类型
  TEXTS,     ///< 文本类型
  INTS,      ///< 整数类型(4字节)
  FLOATS,    ///< 浮点数类型(4字节)
  BOOLEANS,  ///< boolean类型，当前不是由parser解析出来的，是程序内部使用的
};

const char *attr_type_to_string(AttrType type);
AttrType attr_type_from_string(const char *s);

/**
 * @brief 属性的值
 *
 */
class Value
{
public:
  Value() = default;

  Value(AttrType attr_type, char *data, int length = 4) : attr_type_(attr_type) { this->set_data(data, length); }

  explicit Value(int val);
  explicit Value(float val);
  explicit Value(bool val);
  explicit Value(const char *s, int len = 0);
  explicit Value(const char *s, AttrType type);
  explicit Value(AttrType type);

  Value(const Value &other) = default;
  Value &operator=(const Value &other) = default;

  bool operator==(const Value &other) const { return other.compare(*this) == 0; }

  void set_type(AttrType type) { this->attr_type_ = type; }
  void set_data(char *data, int length);
  void set_data(const char *data, int length) { this->set_data(const_cast<char *>(data), length); }
  void set_null();
  void set_int(int val);
  void set_float(float val);
  void set_boolean(bool val);
  void set_string(const char *s, int len = 0);
  void set_date(const char *s);
  void set_date(int date);
  void set_text(const char *s);
  void set_value(const Value &value);

  bool is_null() const { return attr_type_ == AttrType::NULLS; }

  // add 函数
  static Value add(const Value &a, const Value &b)
  {
    if (a.attr_type() == b.attr_type()) {
      // 只支持三种
      switch (a.attr_type()) {
        case AttrType::INTS: return Value(a.get_int() + b.get_int());
        case AttrType::FLOATS: return Value(a.get_float() + b.get_float());
        case AttrType::BOOLEANS: return Value(a.get_int() + b.get_int());
        default: break;
      }
    }
    // TODO: from oldcb 需要修改, 我瞎改的
    if (a.attr_type() == AttrType::NULLS) {
      switch (b.attr_type()) {
        case AttrType::INTS: return Value(b.get_int());
        case AttrType::FLOATS: return Value(b.get_float());
        case AttrType::BOOLEANS: return Value(b.get_int());
        case AttrType::NULLS: return Value(0);
        default: break;
      }
    }
    // TODO: from oldcb 需要修改, 我瞎改的
    if (b.attr_type() == AttrType::NULLS) {
      switch (a.attr_type()) {
        case AttrType::INTS: return Value(a.get_int());
        case AttrType::FLOATS: return Value(a.get_float());
        case AttrType::BOOLEANS: return Value(a.get_int());
        case AttrType::NULLS: return Value(0);
        default: break;
      }
    }
    throw std::runtime_error("Unsupported types for operation.");
  }

  std::string to_string() const;

  int compare(const Value &other) const;

  const char *data() const;
  int length() const { return length_; }

  AttrType attr_type() const { return attr_type_; }

  bool check_date();

public:
  /**
   * 获取对应的值
   * 如果当前的类型与期望获取的类型不符，就会执行转换操作
   */
  int get_int(bool error_is_zero = true) const;  // 如果error_is_zero为true, 那么如果无法转换返回0, 否则为最大值
  float get_float(bool error_is_zero = true) const;  // 如果error_is_zero为true, 那么如果无法转换返回0, 否则为最大值
  std::string get_string() const;
  bool get_boolean() const;

private:
  AttrType attr_type_ = UNDEFINED;
  int length_ = 0;

  union
  {
    int int_value_;
    int date_value_;
    float float_value_;
    bool bool_value_;
  } num_value_;
  std::string str_value_;
};