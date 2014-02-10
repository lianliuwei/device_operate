#pragma once

// TODO move to canscope_device_util.h
#include "base/threading/platform_thread.h"

namespace canscope {
// in us
const int kEnumDevicesInterval = 1000;
const int kOscCollectUpdateStateCount = 5;
const int kOscCollectUpdateStateInterval = 20;
const bool kDefaultAutoInitDevice = true;
const bool kDefaultStartOnDeviceOnline = true;
const bool kDefaultRerunOnbackOnline = true;

inline void SleepMs(int64 ms) {
  base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(ms));
}

enum DeviceType {
  DT_CS1201,
  DT_CS1202,
  DT_CS1203,
};

}; // namespace canscope
