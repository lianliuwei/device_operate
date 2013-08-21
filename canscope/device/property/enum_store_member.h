#pragma once

#include <string>

#include "canscope/device/property/store_member.h"

#define DECLARE_ENUM_STORE_MEMBER(EnumName) \
typedef ::canscope::StoreMember<EnumName> EnumName##StoreMember; \
template <> \
void ::canscope::StoreMember<EnumName>::UpdatePref(const EnumName& value); \
template <> \
bool ::canscope::StoreMember<EnumName>::UpdateValueInternal( \
    const Value& value) const;

// implement two function yourself
// const char* EnumNameToString(EnumName)
// bool StringToEnumName(std::string str, EnumName* value)
#define IMPLEMENT_ENUM_STORE_MEMBER_STRING(EnumName) \
const char* EnumName##ToString(const EnumName& value); \
bool StringTo##EnumName(const std::string& str, EnumName* value); \
template <> \
void ::canscope::StoreMember<EnumName>::UpdatePref(const EnumName& value) { \
  prefs()->SetValue(pref_name(), \
  new base::StringValue(EnumName##ToString(value))); \
} \
template <> \
bool ::canscope::StoreMember<EnumName>::UpdateValueInternal( \
    const base::Value& value) const { \
  std::string str; \
  bool ret = value.GetAsString(&str); \
  if (!ret) \
    return false; \
  return StringTo##EnumName(str, &value_); \
}

#define IMPLEMENT_ENUM_STORE_MEMBER_INT(EnumName) \
template <> \
void ::canscope::StoreMember<EnumName>::UpdatePref(const EnumName& value) { \
  prefs()->SetValue(pref_name(), \
  new base::FundamentalValue(static_cast<int>(value))); \
} \
template <> \
bool ::canscope::StoreMember<EnumName>::UpdateValueInternal( \
    const base::Value& value) const { \
  int int_out; \
  bool ret = value.GetAsInteger(&int_out); \
  if (!ret) \
    return false; \
  value_ = static_cast<EnumName>(int_out); \
  return true; \
}
