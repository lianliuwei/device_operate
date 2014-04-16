#include "testing/gtest/include/gtest/gtest.h"

#include <string>

#include "base/values.h"
#include "base/threading/platform_thread.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"

#include "canscope/device/osc_device/osc_device.h"
#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/test/scoped_open_device.h"
#include "canscope/device/test/test_util.h"

using namespace canscope;
using namespace std;
using namespace base;

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

#define EXPECT_TRUE_OR_RET(ret) \
{ \
  EXPECT_EQ(true, (ret)); \
  if(!(ret)) \
  return; \
}

TEST(OscDeviceTest, WriteChnlConfig) {
  UsbPortDeviceDelegate device_delegate;
  ScopeOpenDevice open_device(&(device_delegate));
  EXPECT_TRUE_OR_RET(open_device.IsOpen());
  OscDevice osc_device(&device_delegate, NULL);

  // config osc_device
  osc_device.Init(GetConfig(kOscConfig));

  osc_device.SetVoltRange(CAN_H);

  bool ret;
//   bool ret = osc_device.Start();
//   EXPECT_TRUE_OR_RET(ret); 
//   base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(200));
//   for (int i = 0; i < 4; ++i) {
//     osc_device.UpdateTriggerState();
//     if (osc_device.IsCollected())
//       break;
//     if (i == 3) 
//       EXPECT_TRUE_OR_RET(false);
//   }
  int data_size = 2000*2;
  //base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(200));
//   scoped_ptr<uint8[]> osc_data(new uint8[data_size]);
//   memset(osc_data.get(), 0, data_size);
//   ret = device_delegate.usb_port.ReadEP3(
//       kScopeReadAddr, kUsbModelStream, osc_data.get(), data_size);
  EXPECT_TRUE_OR_RET(ret); 
}
