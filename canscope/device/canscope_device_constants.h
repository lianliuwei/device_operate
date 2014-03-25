#pragma once

#include "base/threading/platform_thread.h"

namespace canscope {
// in us
const int kEnumDevicesInterval = 1000;
const int kOscCollectUpdateStateCount = 5;
const int kOscCollectUpdateStateInterval = 20;
const bool kDefaultAutoInitDevice = true;
const bool kDefaultStartOnDeviceOnline = true;
const bool kDefaultRerunOnbackOnline = true;

// TODO move to canscope_device_util.h
inline void SleepMs(int64 ms) {
  base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(ms));
}

enum DeviceType {
  DT_CS1201,
  DT_CS1202,
  DT_CS1203,
};

// X K point of Device
inline int DeviceK(DeviceType device_type) {
  return device_type == DT_CS1203 ? 8 : 2;
}

// move to canscope_device.h
inline size_t DeviceTypeDataSize(DeviceType type) {
  // two chnl
  return DeviceK(type) * 1000 * 2;
}

}; // namespace canscope
