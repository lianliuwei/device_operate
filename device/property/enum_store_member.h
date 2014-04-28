#pragma once

#include <string>

#include "base/logging.h"
#include "base/values.h"

#include "device/property/store_member.h"

#define DECLARE_ENUM_STORE_MEMBER(EnumName) \
class EnumName##StoreMember : public ::device::StoreMember<EnumName> { \
public: \
  EnumName##StoreMember() {} \
  virtual ~EnumName##StoreMember() {} \
 \
private: \
  /* implement StoreMember<EnumName> */ \
  virtual bool UpdateValueInternal(const base::Value& value) const OVERRIDE; \
  virtual void UpdatePref(const EnumName& value) OVERRIDE; \
 \
  DISALLOW_COPY_AND_ASSIGN(EnumName##StoreMember); \
};

// implement two function yourself
// const char* EnumNameToString(EnumName)
// bool StringToEnumName(std::string str, EnumName* value)
#define IMPLEMENT_ENUM_STORE_MEMBER_STRING(EnumName) \
const char* EnumName##ToString(const EnumName& value); \
bool StringTo##EnumName(const std::string& str, EnumName* value); \
 \
void EnumName##StoreMember::UpdatePref(const EnumName& value) { \
  prefs()->SetValue(pref_name(), \
  new base::StringValue(EnumName##ToString(value))); \
} \
 \
bool EnumName##StoreMember::UpdateValueInternal( \
    const base::Value& value) const { \
  std::string str; \
  bool ret = value.GetAsString(&str); \
  if (!ret) \
    return false; \
  return StringTo##EnumName(str, &value_); \
}

#define IMPLEMENT_ENUM_STORE_MEMBER_INT(EnumName) \
 \
void EnumName##StoreMember::UpdatePref(const EnumName& value) { \
  prefs()->SetValue(pref_name(), \
  new base::FundamentalValue(static_cast<int>(value))); \
} \
 \
bool EnumName##StoreMember::UpdateValueInternal( \
    const base::Value& value) const { \
  int int_out; \
  bool ret = value.GetAsInteger(&int_out); \
  if (!ret) \
    return false; \
  value_ = static_cast<EnumName>(int_out); \
  return true; \
}
