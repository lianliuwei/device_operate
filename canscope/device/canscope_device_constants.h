#pragma once

#include "base/threading/platform_thread.h"

namespace canscope {
// in us
const int kEnumDevicesInterval = 1000;
const int kOscCollectUpdateStateCount = 5;
const int kOscCollectUpdateStateInterval = 20;
const int kOscCollectMiniTimeLimit = 300; // ms
const bool kDefaultAutoInitDevice = true;
const bool kDefaultStartOnDeviceOnline = true;
const bool kDefaultRerunOnbackOnline = true;
const int kOscMemoryUsage = 100 * 1024* 1024; // 100MB
const int kOscMaxFreq = 30; // real speed can be very high > 500
const int kMaxBaudRate = 100*1000*1000; // 100M
const int kFrameSize = 48;
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

inline uint32 DeviceStorageSize(DeviceType type) {
  return 512 * 1024 * 1024;
}

// (Bit/S)=100000000/((FIL_DIV+1)*16)
inline uint16 BaudRateToFilDiv(int baud_rate) {
  DCHECK(baud_rate > 0);
  return (kMaxBaudRate / baud_rate) / 16 - 1;
}

}; // namespace canscope
