#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "base/bind.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"

#include "canscope/device/property/property.h"
#include "canscope/device/property/property_delegate.h"
#include "canscope/device/property/store_member.h"
#include "canscope/device/property/value_map_device_property_store.h"

using namespace base;
using testing::_;
using testing::Return;

namespace {
static const char* kBoolMember = "test.bool_member";
static const char* kIntMember = "test.int_member";
}

class DeviceThreadMock {
public:
  MOCK_METHOD0(IsDeviceThread, bool());
};

static DeviceThreadMock g_DeviceThreadMock;
namespace canscope {

bool IsDeviceThread() {
  return g_DeviceThreadMock.IsDeviceThread();
}

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

  MOCK_METHOD0(SetBoolMember, void());
  MOCK_METHOD0(SetIntMember, void());

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

  MOCK_METHOD2(bool_property_check, bool(bool, const std::string&));
  MOCK_METHOD2(int_property_check, bool(int, const std::string&));

  // canscope::PropertyDelegate
  virtual canscope::DevicePropertyStore* GetDevicePropertyStore() {
    return &(device_.prefs_);
  }

  virtual bool IsBathMode() {
    return false;
  }

  virtual std::string device_name() {
    return "device";
  }

  MOCK_METHOD1(PostDeviceTask, void(const Closure&));

  virtual void FetchNewPref() {
    prefs_.Reset(device_.prefs_.Serialize());
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
  
  // OneThread no need Post task
  EXPECT_CALL(g_DeviceThreadMock, IsDeviceThread()).WillRepeatedly(Return(true));
  EXPECT_CALL(handle, PostDeviceTask(_)).Times(0);
  
  // get value
  EXPECT_CALL(device, SetBoolMember()).Times(0);
  EXPECT_CALL(device, SetIntMember()).Times(0);
  EXPECT_CALL(handle, bool_property_check(_, _)).Times(0);
  EXPECT_CALL(handle, int_property_check(_, _)).Times(0);

  EXPECT_EQ(false, handle.bool_property.value());
  EXPECT_EQ(3, handle.int_property.value());

  // set from handle
  EXPECT_CALL(device, SetBoolMember()).Times(1);
  EXPECT_CALL(device, SetIntMember()).Times(1);
  EXPECT_CALL(handle, bool_property_check(true, kBoolMember))
      .Times(1).WillOnce(Return(true));
  EXPECT_CALL(handle, int_property_check(44, kIntMember))
      .Times(1).WillOnce(Return(true));

  handle.bool_property.set_value(true);
  EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(true, device.bool_member.value());
  handle.int_property.set_value(44);
  EXPECT_EQ(44, handle.int_property.value());
  EXPECT_EQ(44, device.int_member.value());

  // check false
  EXPECT_CALL(device, SetBoolMember()).Times(0);
  EXPECT_CALL(device, SetIntMember()).Times(0);
  EXPECT_CALL(handle, bool_property_check(false, kBoolMember))
      .Times(1).WillOnce(Return(false));
  EXPECT_CALL(handle, int_property_check(10, kIntMember))
      .Times(1).WillOnce(Return(false));

  handle.bool_property.set_value(false);
  handle.int_property.set_value(10);
  EXPECT_EQ(true, handle.bool_property.value());
  EXPECT_EQ(true, device.bool_member.value());
  EXPECT_EQ(44, handle.int_property.value());
  EXPECT_EQ(44, device.int_member.value());
}

