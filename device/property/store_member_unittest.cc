#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include <string>

#include "device/property/store_member.h"
#include "device/property/value_map_device_property_store.h"

using namespace device;
using namespace base;
using namespace std;
using testing::_;

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
  ChangedCallbackStub() {}
  ~ChangedCallbackStub() {}

  MOCK_METHOD1(Notify, void(const std::string&));
};

TEST(StoreMemberTest, Callback) {
  ValueMapDevicePropertyStore prefs;
  string path("test.property1");
  prefs.SetValue(path, new FundamentalValue(true));
  ChangedCallbackStub stub;
  BooleanStoreMember bool_pref(path.c_str(), &prefs, 
    Bind(&ChangedCallbackStub::Notify, Unretained(&stub)));
  EXPECT_CALL(stub, Notify(_)).Times(0);
  EXPECT_EQ(true, bool_pref.value());
  EXPECT_CALL(stub, Notify(path)).Times(1);
  prefs.SetValue(path, new FundamentalValue(false));
  EXPECT_CALL(stub, Notify(path)).Times(1);
  prefs.SetValue(path, new FundamentalValue(true));
  EXPECT_CALL(stub, Notify(_)).Times(0);
  EXPECT_EQ(true, bool_pref.value());
}
