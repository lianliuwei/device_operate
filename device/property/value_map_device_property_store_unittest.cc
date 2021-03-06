#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include <string>

#include "base/values.h"
#include "base/file_util.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"

#include "device/property/value_map_device_property_store.h"
#include "device/property/device_property_observer_mock.h"

using namespace device;
using namespace base;
using namespace std;
using testing::_;

TEST(ValueMapDevicePropertyStoreTest, SetAndGetValue) {
  ValueMapDevicePropertyStore prefs;
  Value* value = new FundamentalValue(10);
  prefs.SetValue("test.1.2.3", value);
  const Value* ptr = NULL;
  bool ret = prefs.GetValue("test.1.2", &ptr);
  EXPECT_FALSE(ret);
  EXPECT_EQ(NULL, ptr);
  ret = prefs.GetValue("test.1.2.3", &ptr);
  EXPECT_TRUE(ret);
  EXPECT_TRUE(ptr != NULL);
  int temp;
  EXPECT_TRUE(ptr->GetAsInteger(&temp));
  EXPECT_EQ(10, temp);
}

TEST(ValueMapDevicePropertyStoreTest, Observer) {
  DevicePropertyObserverMock stub;
  ValueMapDevicePropertyStore prefs;
  string pref_name = "test.1.2.3";
  prefs.AddPrefObserver(pref_name, &stub);

  EXPECT_CALL(stub, OnPreferenceChanged(pref_name)).Times(1);
  
  Value* value = new FundamentalValue(10);
  prefs.SetValue(pref_name, value);

  testing::Mock::VerifyAndClearExpectations(&stub);


  EXPECT_CALL(stub, OnPreferenceChanged(_)).Times(0);
  value = new FundamentalValue(true);
  string pref_name2 = "test.NoNotify";
  prefs.SetValueSilently(pref_name2, value);

  testing::Mock::VerifyAndClearExpectations(&stub);


  const Value* ptr = NULL;
  bool ret = prefs.GetValue(pref_name2, &ptr);
  EXPECT_TRUE(ret);
  EXPECT_TRUE(ptr != NULL);
  bool temp;
  EXPECT_TRUE(ptr->GetAsBoolean(&temp));
  EXPECT_EQ(true, temp);

  prefs.RemovePrefObserver(pref_name, &stub);
}

namespace {
static const char kTestConfig [] = { " \
{ \
  \"NoMapItem\" : 1, \
  \"test.1.2.3\" : 10, \
  \"test.NoNotify\" : true \
} \
"};

static const char kNewTestConfig [] = { " \
{ \
  \"new item\" : 4, \
  \"test.1.2.3\" : 2, \
  \"test.NoNotify\" : true \
} \
"};

DictionaryValue* GetConfig(const char* config) {
  std::string content(config);
  JSONStringValueSerializer serializer(content);
  int error;
  std::string error_msg;
  Value* value = serializer.Deserialize(&error, &error_msg);
  CHECK(NULL != value);
  CHECK(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);

  return dict_value;
}

}

TEST(ValueMapDevicePropertyStoreTest, ReadConfigFile) {
  ValueMapDevicePropertyStore prefs;
  prefs.Reset(GetConfig(kTestConfig));

  string pref_name = "test.1.2.3";
  const Value* value_out;
  prefs.GetValue(pref_name, &value_out);
  EXPECT_TRUE(value_out->IsType(Value::TYPE_INTEGER));
  int int_out;
  bool ret = value_out->GetAsInteger(&int_out);
  EXPECT_TRUE(ret);
  EXPECT_EQ(10, int_out);
 
  string pref_name2 = "test.NoNotify";
  prefs.GetValue(pref_name2, &value_out);
  EXPECT_TRUE(value_out->IsType(Value::TYPE_BOOLEAN));
  bool bool_out;
  ret = value_out->GetAsBoolean(&bool_out);
  EXPECT_TRUE(ret);
  EXPECT_EQ(true, bool_out);
}

TEST(ValueMapDevicePropertyStoreTest, ChangedSet) {
  ValueMapDevicePropertyStore prefs;
  prefs.Reset(GetConfig(kTestConfig));

  ValueMapDevicePropertyStore new_prefs;
  new_prefs.Reset(GetConfig(kNewTestConfig));

  vector<string> change_set = prefs.ChangedSet(&new_prefs);
  EXPECT_EQ(2, change_set.size());
  EXPECT_NE(change_set.end(), find(change_set.begin(), change_set.end(), "NoMapItem"));
  EXPECT_NE(change_set.end(), find(change_set.begin(), change_set.end(), "test.1.2.3"));
}