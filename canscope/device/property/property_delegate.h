#pragma once

#include <string>

#include "base/callback.h"

#include "device/property/device_property_store.h"

namespace canscope {

class PropertyDelegate {
public:
  virtual ::device::DevicePropertyStore* GetDevicePropertyStore() = 0;
  // bath mode config device at once
  virtual bool IsBatchMode() = 0;

  virtual void PostDeviceTask(const base::Closure& task) = 0;
  virtual bool IsDeviceThread() = 0;

  virtual void FetchNewPref() = 0;

  // call on device thread
  virtual void SetPropertyFinish(const std::string& reason) = 0;
protected:
  PropertyDelegate() {}
  virtual ~PropertyDelegate() {}
};

} // namespace canscope
