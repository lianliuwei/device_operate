#include "testing/gtest/include/gtest/gtest.h"

#include <string>
#include <tchar.h>

#include "base/file_util.h"
#include "base/values.h"
#include "base/threading/platform_thread.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"

#include "canscope/device/osc_device.h"
#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/register/device_info.h"
#include "canscope/device/register/canscope_device_register_constants.h"

using namespace canscope;
using namespace std;
using namespace base;

#define EXPECT_TRUE_OR_RET(ret) \
  { \
    EXPECT_EQ(true, (ret)); \
    if(!(ret)) \
    return; \
  }

#define EXPECT_TRUE_OR_RET_FALSE(ret) \
  { \
    EXPECT_EQ(true, (ret)); \
    if(!(ret)) \
    return false; \
  }

class ScopeOpenDevice {
public:
  ScopeOpenDevice(UsbPortDeviceDelegate* device_delegate)
      : device_delegate_(device_delegate)
      , open_(false) {
    DCHECK(device_delegate);
    open_ = InitDevice(device_delegate);
  }

  ~ScopeOpenDevice() {
    if (open_)
      CloseDevice(device_delegate_->usb_port_ptr());
  }

  bool IsOpen() const {
    return open_;
  }

private:
  static bool InitDevice(UsbPortDeviceDelegate* device_delegate) {
    UsbPort* usb_port = device_delegate->usb_port_ptr();

    std::vector<string16> devices;
    bool ret = EnumDevices(&devices);
    EXPECT_TRUE_OR_RET_FALSE(ret && devices.size() > 0);

    ret = usb_port->OpenDevice(devices[0]);
    EXPECT_TRUE_OR_RET_FALSE(ret);

    DeviceInfo device_info;
    ret = device_delegate->GetDeviceInfo(&device_info);
    EXPECT_TRUE_OR_RET_FALSE(ret);
    // config FPGA
    if (device_info.fpage_version.value() == 0xFFFFFFFF) {
      std::string content;
      ret = file_util::ReadFileToString(
        base::FilePath(_T("CANScope.dll")), &content); 
      EXPECT_TRUE_OR_RET_FALSE(ret);
      ret = usb_port->DownloadFPGA((uint8*)(content.c_str()), content.size());
      EXPECT_TRUE_OR_RET_FALSE(ret);
    }
    return true;
  }

  static void CloseDevice(UsbPort* usb_port) {
    usb_port->CloseDevice();
  }

  UsbPortDeviceDelegate* device_delegate_;
  bool open_;

  DISALLOW_COPY_AND_ASSIGN(ScopeOpenDevice);
};
namespace {
static const char kOscConfig [] =  {" \
{ \
  \"CANH.Range\" : 3, \
  \"CANH.Offset\" : 0.0, \
  \"CANH.Coupling\" : 1, \
  \"CANL.RangeCANH\" : 3, \
  \"CANL.OffsetCANH\" : 0.0, \
  \"CANL.Coupling\" : 1, \
  \"CANDIFF.Range\" : 3, \
  \"CANDIFF.Offset\" : 0.0, \
  \"DiffCtrl\" : 0, \
  \"Time.Base\" : 4, \
  \"Time.Offset\" : 0.0, \
  \"Trigger.AutoTime\" : 100.0, \
  \"Trigger.Source\" : 2, \
  \"Trigger.Type\" : 0, \
  \"Trigger.Mode\" : 0, \
  \"Trigger.Sens\" : 0, \
  \"Trigger.Compare\" : 0, \
  \"Trigger.Volt\" : 0.0, \
  \"Trigger.Param\" : 1.0 \
} \
"};
}
TEST(OscDeviceTest, WriteChnlConfig) {
  UsbPortDeviceDelegate device_delegate;
  ScopeOpenDevice open_device(&(device_delegate));
  EXPECT_TRUE_OR_RET(open_device.IsOpen());
  OscDevice osc_device(&device_delegate);

  // config osc_device
  string content(kOscConfig);
  JSONStringValueSerializer serializer(content);
  int error;
  string error_msg;
  Value* value = serializer.Deserialize(&error, &error_msg);
  EXPECT_TRUE(NULL != value);
  EXPECT_TRUE(value->IsType(Value::TYPE_DICTIONARY));
  ValueMapDevicePropertyStore prefs;
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);
  osc_device.Init(dict_value);

  osc_device.SetVoltRange(CAN_H);

  bool ret = osc_device.Start();
  EXPECT_TRUE_OR_RET(ret); 
  base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(200));
  for (int i = 0; i < 4; ++i) {
    osc_device.UpdateTriggerState();
    if (osc_device.IsCollected())
      break;
    if (i == 3) 
      EXPECT_TRUE_OR_RET(false);
  }
  int data_size = 2000*2;
  //base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(200));
  scoped_ptr<uint8[]> osc_data(new uint8[data_size]);
  memset(osc_data.get(), 0, data_size);
  ret = device_delegate.usb_port.ReadEP3(
      kScopeReadAddr, kUsbModelStream, osc_data.get(), data_size);
  EXPECT_TRUE_OR_RET(ret); 
}