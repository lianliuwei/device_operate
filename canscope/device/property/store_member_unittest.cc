#include "testing/gtest/include/gtest/gtest.h"

#include "canscope/device/property/store_member.h"
#include "canscope/device/property/value_map_device_property_store.h"

using namespace canscope;
using namespace base;
using namespace std;

TEST(StoreMemberTest, BoolMember) {
  ValueMapDevicePropertyStore prefs;
  string path("test.property1");
  prefs.SetValue(path, new FundamentalValue(true));
  StoreMember<bool> bool_pref(path.c_str(), &prefs);
  EXPECT_EQ(true, bool_pref.value());
  bool_pref.set_value(false);
  const Value* value;
  bool ret = prefs.GetValue(path, &value);
  EXPECT_TRUE(ret);
  bool bool_out;
  EXPECT_TRUE(value->GetAsBoolean(&bool_out));
  EXPECT_FALSE(bool_out);
 prefs.SetValue(path, new FundamentalValue(false)); 
  EXPECT_EQ(false, bool_pref.value());
}