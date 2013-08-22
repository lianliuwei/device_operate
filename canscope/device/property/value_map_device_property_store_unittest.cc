#include "testing/gtest/include/gtest/gtest.h"

#include <string>

#include "base/values.h"
#include "base/file_util.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"

#include "canscope/device/property/value_map_device_property_store.h"

using namespace canscope;
using namespace base;
using namespace std;

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

class DevicePropertyObserverStub : public DevicePropertyStore::Observer {
public:
  DevicePropertyObserverStub() 
      : count(0) {}
  virtual ~DevicePropertyObserverStub() {}

  virtual void OnPreferenceChanged(const std::string& pref_name) {
    ++count;
    last_pref_name = pref_name;
  }
 
  int count;
  string last_pref_name;
};

TEST(ValueMapDevicePropertyStoreTest, Observer) {
  DevicePropertyObserverStub stub;
  EXPECT_EQ(0, stub.count);
  EXPECT_EQ("", stub.last_pref_name);
  ValueMapDevicePropertyStore prefs;
  string pref_name = "test.1.2.3";
  prefs.AddPrefObserver(pref_name, &stub);
  Value* value = new FundamentalValue(10);
  prefs.SetValue(pref_name, value);
  EXPECT_EQ(1, stub.count);
  EXPECT_EQ(pref_name, stub.last_pref_name);
  value = new FundamentalValue(true);
  string pref_name2 = "test.NoNotify";
  prefs.SetValueSilently(pref_name2, value);
  EXPECT_EQ(1, stub.count);
  EXPECT_EQ(pref_name, stub.last_pref_name);
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
  \"test.1.2.3\" : 10, \
  \"test.NoNotify\" : true \
} \
"};

}

TEST(ValueMapDevicePropertyStoreTest, ReadWriteConfigFile) {
  string config(kTestConfig);
  JSONStringValueSerializer serializer(config);
  int error;
  string error_msg;
  Value* value = serializer.Deserialize(&error, &error_msg);
  EXPECT_TRUE(NULL != value);
  EXPECT_TRUE(value->IsType(Value::TYPE_DICTIONARY));
  ValueMapDevicePropertyStore prefs;
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);
  prefs.Reset(dict_value);
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