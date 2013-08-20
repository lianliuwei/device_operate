#include "testing/gtest/include/gtest/gtest.h"

#include <string>

#include "canscope/device/property/store_member.h"
#include "canscope/device/property/value_map_device_property_store.h"

using namespace canscope;
using namespace base;
using namespace std;

TEST(StoreMemberTest, BoolMember) {
  ValueMapDevicePropertyStore prefs;
  string path("test.property1");
  prefs.SetValue(path, new FundamentalValue(true));
  BooleanStoreMember bool_pref(path.c_str(), &prefs);
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

class ChangedCallbackStub {
public:
  ChangedCallbackStub() 
      : count(0) {}
  ~ChangedCallbackStub() {}
  void Notify(const std::string& pref_name) {
     ++count;
     last_pref_name = pref_name;
  }
  int count;
  string last_pref_name;
};

TEST(StoreMemberTest, Callback) {
  ValueMapDevicePropertyStore prefs;
  string path("test.property1");
  prefs.SetValue(path, new FundamentalValue(true));
  ChangedCallbackStub stub;
  BooleanStoreMember bool_pref(path.c_str(), &prefs, 
    Bind(&ChangedCallbackStub::Notify, Unretained(&stub)));
  EXPECT_EQ(0, stub.count);
  EXPECT_EQ("", stub.last_pref_name);
  EXPECT_EQ(true, bool_pref.value());
  EXPECT_EQ(0, stub.count);
  EXPECT_EQ("", stub.last_pref_name);
  prefs.SetValue(path, new FundamentalValue(false));
  EXPECT_EQ(1, stub.count);
  EXPECT_EQ(path, stub.last_pref_name);
  prefs.SetValue(path, new FundamentalValue(true));
  EXPECT_EQ(2, stub.count);
  EXPECT_EQ(path, stub.last_pref_name);
  EXPECT_EQ(true, bool_pref.value());
  EXPECT_EQ(2, stub.count);
  EXPECT_EQ(path, stub.last_pref_name);
}