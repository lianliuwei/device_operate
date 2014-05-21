#pragma once

#include "canscope/device/canscope_device_constants.h"

namespace canscope {

extern const void* kCANHKey;
extern const void* kCANLKey;
extern const void* kCANDIFFKey;

const int kChnlVDiv = 8;
const int kMaxChnlHDiv = 10; // max show h div

inline int DeviceHDiv(DeviceType type) {
  return type == DT_CS1203 ? 40 : 10;
}
} // namespace canscope