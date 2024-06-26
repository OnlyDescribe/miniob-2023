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
// Created by WangYunlai on 2023/06/28.
//

#include <cmath>
#include <sstream>
#include <iomanip>
#include "sql/parser/value.h"
#include "storage/field/field.h"
#include "common/log/log.h"
#include "common/lang/comparator.h"
#include "common/lang/string.h"

// 注意这里的顺序应该与enum AttrType一一对应
const char *ATTR_TYPE_NAME[] = {"undefined", "nulls", "dates", "chars", "texts", "ints", "floats", "booleans"};

const char *attr_type_to_string(AttrType type)
{
  if (type >= UNDEFINED && type <= FLOATS) {  // 新添加字段的时候搜索范围
    return ATTR_TYPE_NAME[type];
  }
  return "unknown";
}
AttrType attr_type_from_string(const char *s)
{
  for (unsigned int i = 0; i < sizeof(ATTR_TYPE_NAME) / sizeof(ATTR_TYPE_NAME[0]); i++) {
    if (0 == strcmp(ATTR_TYPE_NAME[i], s)) {
      return (AttrType)i;
    }
  }
  return UNDEFINED;
}

Value::Value(int val) { set_int(val); }

Value::Value(float val) { set_float(val); }

Value::Value(bool val) { set_boolean(val); }

Value::Value(const char *s, int len /*= 0*/) { set_string(s, len); }

Value::Value(const char *s, AttrType type)
{
  if (type == AttrType::DATES) {
    set_date(s);
  } else {
    LOG_WARN("unknown data type: %d", type);
  }
}

Value::Value(AttrType type)
{
  if (type == AttrType::NULLS) {
    set_null();
  } else {
    LOG_WARN("unknown data type: %d", type);
  }
}

void Value::set_data(char *data, int length)
{
  switch (attr_type_) {
    case CHARS: {
      set_string(data, length);
    } break;
    case TEXTS: {
      set_text(data);
    } break;
    case DATES: {
      // set_date(data);
      num_value_.date_value_ = *(int *)data;
      length_ = length;
    } break;
    case INTS: {
      num_value_.int_value_ = *(int *)data;
      length_ = length;
    } break;
    case FLOATS: {
      num_value_.float_value_ = *(float *)data;
      length_ = length;
    } break;
    case BOOLEANS: {
      num_value_.bool_value_ = *(int *)data != 0;
      length_ = length;
    } break;
    default: {
      LOG_WARN("unknown data type: %d", attr_type_);
    } break;
  }
}
void Value::set_null()
{
  attr_type_ = NULLS;
  num_value_.int_value_ = 0;
  length_ = 0;
}

void Value::set_int(int val)
{
  attr_type_ = INTS;
  num_value_.int_value_ = val;
  length_ = sizeof(val);
}

void Value::set_float(float val)
{
  attr_type_ = FLOATS;
  num_value_.float_value_ = val;
  length_ = sizeof(val);
}
void Value::set_boolean(bool val)
{
  attr_type_ = BOOLEANS;
  num_value_.bool_value_ = val;
  length_ = sizeof(val);
}
void Value::set_string(const char *s, int len /*= 0*/)
{
  attr_type_ = CHARS;
  if (len > 0) {
    len = strnlen(s, len);
    str_value_.assign(s, len);
  } else {
    str_value_.assign(s);
  }
  length_ = str_value_.length();
}

void Value::set_date(const char *s)
{
  attr_type_ = DATES;
  str_value_.assign(s);

  int year, month, day;
  int ret = sscanf(s, "%d-%d-%d", &year, &month, &day);
  ASSERT(ret != 3, "INVALID_ARGUMENT");

  num_value_.date_value_ = year * 10000 + month * 100 + day;
  length_ = sizeof(num_value_.date_value_);
}

void Value::set_date(int date)
{
  attr_type_ = DATES;
  num_value_.date_value_ = date;
  length_ = sizeof(date);
}

void Value::set_text(const char *s)
{
  attr_type_ = TEXTS;
  str_value_.assign(s);
  length_ = str_value_.length();
}

void Value::set_value(const Value &value)
{
  switch (value.attr_type_) {
    case INTS: {
      set_int(value.get_int());
    } break;
    case FLOATS: {
      set_float(value.get_float());
    } break;
    case CHARS: {
      set_string(value.get_string().c_str());
    } break;
    case TEXTS: {
      set_text(value.get_string().c_str());
    } break;
    case BOOLEANS: {
      set_boolean(value.get_boolean());
    } break;
    case DATES: {
      if (attr_type_ == value.attr_type_) {
        set_date(value.str_value_.c_str());
      }
    } break;
    case UNDEFINED: {
      ASSERT(false, "got an invalid value type");
    } break;
    default: break;
  }
}

const char *Value::data() const
{
  switch (attr_type_) {
    case CHARS:
    case TEXTS: {
      return str_value_.c_str();
    } break;
    default: {
      return (const char *)&num_value_;
    } break;
  }
}

std::string Value::to_string() const
{
  std::stringstream os;
  switch (attr_type_) {
    case NULLS: {
      os << "NULL";
    } break;
    case INTS: {
      os << num_value_.int_value_;
    } break;
    case FLOATS: {
      os << common::double_to_str(num_value_.float_value_);
    } break;
    case BOOLEANS: {
      os << num_value_.bool_value_;
    } break;
    case CHARS:
    case TEXTS: {
      os << str_value_;
    } break;
    case DATES: {
      int date = num_value_.date_value_;
      int year = date / 10000;
      int month = (date % 10000) / 100;
      int day = date % 100;

      os << year << "-" << std::setfill('0') << std::setw(2) << month << "-" << std::setfill('0') << std::setw(2)
         << day;
    } break;
    default: {
      LOG_WARN("unsupported attr type: %d", attr_type_);
    } break;
  }
  return os.str();
}

int Value::compare(const Value &other) const
{
  if (this->attr_type_ == other.attr_type_) {
    switch (this->attr_type_) {
      case INTS: {
        return common::compare_int((void *)&this->num_value_.int_value_, (void *)&other.num_value_.int_value_);
      } break;
      case FLOATS: {
        return common::compare_float((void *)&this->num_value_.float_value_, (void *)&other.num_value_.float_value_);
      } break;
      case CHARS:
      case TEXTS: {
        return common::compare_string((void *)this->str_value_.c_str(),
            this->str_value_.length(),
            (void *)other.str_value_.c_str(),
            other.str_value_.length());
      } break;
      case BOOLEANS: {
        return common::compare_int((void *)&this->num_value_.bool_value_, (void *)&other.num_value_.bool_value_);
      }
      case DATES: {
        return common::compare_date((void *)&this->num_value_.date_value_, (void *)&other.num_value_.date_value_);
      }
      default: {
        LOG_WARN("unsupported type: %d", this->attr_type_);
      }
    }
  } else if (this->attr_type_ == INTS && other.attr_type_ == FLOATS) {
    float this_data = this->num_value_.int_value_;
    return common::compare_float((void *)&this_data, (void *)&other.num_value_.float_value_);
  } else if (this->attr_type_ == FLOATS && other.attr_type_ == INTS) {
    float other_data = other.num_value_.int_value_;
    return common::compare_float((void *)&this->num_value_.float_value_, (void *)&other_data);
  }
  // common comparison for DATES and INTS and FLOATS
  else if ((this->attr_type_ == DATES || this->attr_type_ == INTS || this->attr_type_ == FLOATS) &&
           (other.attr_type_ == DATES || other.attr_type_ == INTS || other.attr_type_ == FLOATS)) {
    float this_data = this->get_float();
    float other_data = other.get_float();
    return common::compare_float((void *)&this_data, (void *)&other_data);
  }
  // common comparison for CHARS and TEXTS
  else if ((this->attr_type_ == CHARS || this->attr_type_ == TEXTS) &&
           (other.attr_type_ == CHARS || other.attr_type_ == TEXTS)) {
    return common::compare_string((void *)this->str_value_.c_str(),
        this->str_value_.length(),
        (void *)other.str_value_.c_str(),
        other.str_value_.length());
  }
  // common comparison for NULLS
  else if (this->attr_type_ == NULLS || other.attr_type_ == NULLS) {
    LOG_WARN("can not compare");
  } else if ((this->attr_type_ == CHARS && (other.attr_type_ == FLOATS || other.attr_type_ == INTS)) ||
             (other.attr_type_ == CHARS && (this->attr_type_ == FLOATS || this->attr_type_ == INTS))) {
    if (attr_type_ == CHARS) {
      return common::compare_string_and_num(this->str_value_, other.get_float());
    } else {
      return -common::compare_string_and_num(other.get_string(), this->get_float());
    }
  }
  LOG_WARN("not supported");
  return -1;  // TODO return rc?
}

// 如果error_is_zero为true, 那么如果无法转换返回0, 否则为最大值
int Value::get_int(bool error_is_zero) const
{
  switch (attr_type_) {
    case CHARS:
    case TEXTS: {
      try {
        // return (int)(std::stol(str_value_));
        return std::round(std::stof(str_value_));
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to number. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return error_is_zero ? 0 : std::numeric_limits<int>::max();
      }
    }
    case INTS: {
      return num_value_.int_value_;
    }
    case FLOATS: {
      return (int)(std::round(num_value_.float_value_));
    }
    case BOOLEANS: {
      return (int)(num_value_.bool_value_);
    }
    case DATES: {
      return (int)(num_value_.date_value_);
    }
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

// 如果error_is_zero为true, 那么如果无法转换返回0, 否则为最大值
float Value::get_float(bool error_is_zero) const
{
  switch (attr_type_) {
    case CHARS:
    case TEXTS: {
      try {
        return std::stof(str_value_);
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return error_is_zero ? 0.0 : std::numeric_limits<float>::max();
      }
    } break;
    case INTS: {
      return float(num_value_.int_value_);
    } break;
    case FLOATS: {
      return num_value_.float_value_;
    } break;
    case BOOLEANS: {
      return float(num_value_.bool_value_);
    } break;
    case DATES: {
      return (float)(num_value_.date_value_);
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return 0;
    }
  }
  return 0;
}

std::string Value::get_string() const { return this->to_string(); }

bool Value::get_boolean() const
{
  switch (attr_type_) {
    case CHARS:
    case TEXTS: {
      try {
        float val = std::stof(str_value_);
        if (val >= EPSILON || val <= -EPSILON) {
          return true;
        }

        int int_val = std::stol(str_value_);
        if (int_val != 0) {
          return true;
        }

        return !str_value_.empty();
      } catch (std::exception const &ex) {
        LOG_TRACE("failed to convert string to float or integer. s=%s, ex=%s", str_value_.c_str(), ex.what());
        return !str_value_.empty();
      }
    } break;
    case INTS: {
      return num_value_.int_value_ != 0;
    } break;
    case FLOATS: {
      float val = num_value_.float_value_;
      return val >= EPSILON || val <= -EPSILON;
    } break;
    case BOOLEANS: {
      return num_value_.bool_value_;
    } break;
    case DATES: {
      return false;
    } break;
    default: {
      LOG_WARN("unknown data type. type=%d", attr_type_);
      return false;
    }
  }
  return false;
}

bool Value::check_date()
{
  int date = num_value_.date_value_;
  int year = date / 10000;
  int month = (date % 10000) / 100;
  int day = date % 100;

  static int mon[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  bool leap = (year % 400 == 0 || (year % 100 && year % 4 == 0));
  return year > 0 && (month > 0) && (month <= 12) && (day > 0) && (day <= ((month == 2 && leap) ? 1 : 0) + mon[month]);
}
