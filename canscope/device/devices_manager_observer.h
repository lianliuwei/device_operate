#pragma once

namespace canscope {

enum DeviceStatus {
  kExist,
  kOnline,
  kOffline,
  kNoExist,
//  Device Collecter status
//   kStart,
//   kStop,
};

class DevicesManagerObserver {
public:
  // offline <-> online
  virtual void DeviceStateChanged() = 0;
  // include DeviceStateChanged
  virtual void DeviceListChanged() = 0;

protected:
  DevicesManagerObserver() {}
  virtual ~DevicesManagerObserver() {}
};

} // namespace canscope
