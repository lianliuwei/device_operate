#include "testing/gtest/include/gtest/gtest.h"

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