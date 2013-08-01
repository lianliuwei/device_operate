#include "testing/gtest/include/gtest/gtest.h"

#include <tchar.h>

#include "base/file_util.h"

#include "canscope/device/usb/usb_port.h"
#include "canscope/device/usb/device_info.h"

using namespace canscope;

TEST(UsbPortTest, EnumDevices) {
  std::vector<string16> devices;
  bool ret = EnumDevices(&devices);
  EXPECT_EQ(true, ret);
}

TEST(UsbPortTest, OpenDevice) {
  std::vector<string16> devices;
  bool ret = EnumDevices(&devices);
  EXPECT_EQ(true, ret);
  UsbPort usb_port;
  ret = usb_port.OpenDevice(devices[0]);
  EXPECT_EQ(true, ret);
  DeviceInfo device_info;
  ret = usb_port.GetDeviceInfo(device_info.memory.buffer(), 
      device_info.memory.size());
  EXPECT_EQ(true, ret);
}

#define EXPECT_TRUE_OR_RET(ret) \
    EXPECT_EQ(true, (ret)); \
    if(!(ret)) \
      return

          
TEST(UsbPortTest, DownloadFPGA) {
  std::string content;
  bool ret = file_util::ReadFileToString(
      base::FilePath(_T("CANScope.dll")), &content); 
  EXPECT_TRUE_OR_RET(ret);
  std::vector<string16> devices;
  ret = EnumDevices(&devices);
  EXPECT_TRUE_OR_RET(ret && devices.size() > 0);
  UsbPort usb_port;
  ret = usb_port.OpenDevice(devices[0]);
  EXPECT_TRUE_OR_RET(ret);
  ret = usb_port.DownloadFPGA((uint8*)(content.c_str()), content.size());
  EXPECT_TRUE_OR_RET(ret);
}