#pragma once

#include "canscope/device/device_delegate.h"
#include "canscope/device/canscope_device_constants.h"

namespace canscope {

// open the first Device of DeviceList. if fault call gtest EXPECT_
class ScopedOpenDevice {
public:
  ScopedOpenDevice(DeviceDelegate* device_delegate);
  ~ScopedOpenDevice();

  bool IsOpen() const { return open_; }
  DeviceType type() const  { return type_; }

private:
  static bool InitDevice(DeviceDelegate* device_delegate, DeviceType* type);

  static void CloseDevice(DeviceDelegate* device_delegate) {
    device_delegate->CloseDevice();
  }

  DeviceDelegate* device_delegate_;
  bool open_;
  DeviceType type_;

  DISALLOW_COPY_AND_ASSIGN(ScopedOpenDevice);
};

} // namespace canscope