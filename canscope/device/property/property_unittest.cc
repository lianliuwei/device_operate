#include "testing/gtest/include/gtest/gtest.h"

#include "base/bind.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"

#include "canscope/device/property/property.h"
#include "canscope/device/property/property_delegate.h"
#include "canscope/device/property/store_member.h"
#include "canscope/device/property/value_map_device_property_store.h"

using namespace base;

namespace {
static const char* kBoolMember = "test.bool_member";
static const char* kIntMember = "test.int_member";
}

class Device {
public:
  Device() {}
  ~Device() {}

  void Init(base::DictionaryValue* dict) {
    prefs_.Reset(dict);

    bool_member.Init(kBoolMember, &prefs_);
    int_member.Init(kIntMember, &prefs_);
  }

  void SetBoolMember() {}
  void SetIntMember() {}

  canscope::ValueMapDevicePropertyStore prefs_;

  canscope::BooleanStoreMember bool_member;
  canscope::IntegerStoreMember int_member;

};

class DeviceHandle : public Device
                   , public canscope::PropertyDelegate {
public:
  DeviceHandle(Device& device)
      : bool_property(this, kBoolMember, bool_member, device.bool_member,
          Bind(&Device::SetBoolMember, Unretained(&device)),
          Bind(&DeviceHandle::bool_property_check, Unretained(this)))
      , int_property(this, kIntMember, int_member, device.int_member,
          Bind(&Device::SetIntMember, Unretained(&device)),
          Bind(&DeviceHandle::int_property_check, Unretained(this)))
      , device_(device) {
    Init(device.prefs_.Serialize());
  }
  virtual ~DeviceHandle() {

  }

  bool bool_property_check(bool value, const std::string& str) {
    return true;
  }

  bool int_property_check(int value, const std::string& str) {
    return true;
  }

  // canscope::PropertyDelegate
  virtual canscope::DevicePropertyStore* GetDevicePropertyStore() {
    return &(device_.prefs_);
  }

  virtual bool IsBathMode() {
    return false;
  }

  virtual std::string device_name() {
    return "string";
  }

  virtual void PostDeviceTask(const Closure& task) {
    NOTIMPLEMENTED();
  }

  canscope::BooleanProperty bool_property;
  canscope::IntegerProperty int_property;

private:
  Device& device_;
};

namespace {
static const char kTestConfig[] =  {" \
{ \
  \"test.bool_member\" : false, \
  \"test.int_member\" : 3 \
} \
"};
}

DictionaryValue* GetDefaultConfig() {
  std::string content(kTestConfig);
  JSONStringValueSerializer serializer(content);
  Value* value = serializer.Deserialize(NULL, NULL);
  EXPECT_TRUE(NULL != value);
  EXPECT_TRUE(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);
  return dict_value;
}

TEST(PropertyTest, OneThread) {
  Device device;
  device.Init(GetDefaultConfig());
  DeviceHandle handle(device);
  // get value
  EXPECT_EQ(false, handle.bool_property.value());
  EXPECT_EQ(3, handle.int_property.value());
  // set from handle
  handle.bool_property.set_value(true);
  EXPECT_EQ(true, device.bool_member.value());
  handle.int_property.set_value(44);
  EXPECT_EQ(44, device.int_member.value());
}

namespace canscope {
bool IsDeviceThread() {
  return true;
}
}