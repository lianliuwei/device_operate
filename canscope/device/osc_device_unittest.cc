#include "testing/gtest/include/gtest/gtest.h"

#include <tchar.h>

#include "base/file_util.h"
#include "base/threading/platform_thread.h"

#include "canscope/device/osc_device.h"
#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/register/device_info.h"
#include "canscope/device/canscope_device_constants.h"

using namespace canscope;

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
TEST(OscDeviceTest, WriteChnlConfig) {
  UsbPortDeviceDelegate device_delegate;
  ScopeOpenDevice open_device(&(device_delegate));
  EXPECT_TRUE_OR_RET(open_device.IsOpen());
  OscDevice osc_device(&device_delegate);

  // config osc_device
  osc_device.chnl_configs[CAN_H].range = k8V;
  osc_device.chnl_configs[CAN_H].offset = 0;
  osc_device.chnl_configs[CAN_H].offset = kAC;

  osc_device.chnl_configs[CAN_L].range = k8V;
  osc_device.chnl_configs[CAN_L].offset = 0;
  osc_device.chnl_configs[CAN_L].offset = kAC;

  osc_device.chnl_configs[CAN_DIFF].range = k8V;
  osc_device.chnl_configs[CAN_DIFF].offset = 0;
  osc_device.chnl_configs[CAN_DIFF].offset = kAC;

  osc_device.diff_ctrl = kSub;

  osc_device.time_base = k20us;
  osc_device.time_offset = 0.0;

  osc_device.trigger_source = kTriggerSourceCANDIFF;
  osc_device.trigger_type = kRisingEdge;
  osc_device.trigger_volt = 0;
  osc_device.compare = kGreater;
  osc_device.trigger_mode = kAuto;
  osc_device.auto_time = 100.0;
  osc_device.trigger_sens = kDefault;
  osc_device.time_param = 1;

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