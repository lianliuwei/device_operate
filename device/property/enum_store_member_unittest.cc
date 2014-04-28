#include "testing/gtest/include/gtest/gtest.h"

#include "device/property/enum_store_member.h"
#include "device/property/value_map_device_property_store.h"

enum EnumType1 {
  kTestEnum1,
  kTestEnum2,
  kTestEnum3,
};

enum EnumType2 {
  kTestEnum4,
  kTestEnum5,
  kTestEnum6,
};

namespace device {
enum EnumType3 {
  kTestEnum7,
  kTestEnum8,
  kTestEnum9,
};
}

namespace device {
DECLARE_ENUM_STORE_MEMBER(EnumType1)
};

namespace device {
IMPLEMENT_ENUM_STORE_MEMBER_STRING(EnumType1)
}

DECLARE_ENUM_STORE_MEMBER(EnumType2)
IMPLEMENT_ENUM_STORE_MEMBER_INT(EnumType2)

namespace device {
DECLARE_ENUM_STORE_MEMBER(EnumType3)
IMPLEMENT_ENUM_STORE_MEMBER_INT(EnumType3)
}

// can't no use in other namespace
//namespace test_namespace {
//DECLARE_ENUM_STORE_MEMBER(EnumType3)
//IMPLEMENT_ENUM_STORE_MEMBER(EnumType3)
//}

// same namespace as DECLARE
namespace device {
const char* EnumType1ToString(const EnumType1& value) {
  switch (value) {
  case kTestEnum1: return "kTestEnum1";
  case kTestEnum2: return "kTestEnum2";
  case kTestEnum3: return "kTestEnum3";
  default: NOTREACHED(); return "";
  }
}

bool StringToEnumType1(const std::string& str, EnumType1* value) {
  if (str == "kTestEnum1") {
    *value = kTestEnum1;
    return true;
  } else if (str == "kTestEnum2") {
    *value = kTestEnum2;
    return true;
  } else if (str == "kTestEnum3") {
    *value = kTestEnum3;
    return true;
  }
  return false;
}

} // namespace device


TEST(EnumTypeStoreMemberTest, CheckString) {
  std::string path("test.property1");
  ::device::ValueMapDevicePropertyStore prefs;
  prefs.SetValue(path, new StringValue("kTestEnum1"));
  ::device::EnumType1StoreMember store;
  store.Init(path.c_str(), &prefs);
  EXPECT_EQ(kTestEnum1, store.value());
  store.set_value(kTestEnum2);
  EXPECT_EQ(kTestEnum2, store.value());
  const Value* value;
  bool ret = prefs.GetValue(path, &value);
  EXPECT_TRUE(ret);
  std::string string_out;
}

TEST(EnumTypeStoreMemberTest, CheckInt) {
  ::device::ValueMapDevicePropertyStore prefs;
  const Value* value;
  std::string path("test.property2");
  prefs.SetValue(path, new base::FundamentalValue(0));
  EnumType2StoreMember store;
  store.Init(path.c_str(), &prefs);
  EXPECT_EQ(kTestEnum4, store.value());
  store.set_value(kTestEnum5);
  bool ret = prefs.GetValue(path, &value);
  EXPECT_TRUE(ret);
  int int_out;
  EXPECT_TRUE(value->GetAsInteger(&int_out));
  EXPECT_EQ(1, int_out);
}