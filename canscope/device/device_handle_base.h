#pragma once

#include "common/notification/notification_observer.h"
#include "common/notification/notification_registrar.h"

#include "canscope/device/device_base.h"
#include "canscope/device/property/property_delegate.h"

namespace canscope {
class ValueMapDevicePropertyStore;
}

class DeviceHandleBase : public canscope::PropertyDelegate
                       , public common::NotificationObserver {
public:
  DeviceHandleBase(DeviceBase* device);
  virtual ~DeviceHandleBase() {}

  // PropertyDelegate
  virtual canscope::DevicePropertyStore* GetDevicePropertyStore();
  virtual bool IsBatchMode() { return batch_mode_; }
  virtual void PostDeviceTask(const base::Closure& task);
  virtual void FetchNewPref();
  virtual void SetPropertyFinish(const std::string& reason);

  void SetBatchMode(bool value) { batch_mode_ = value; }
  
  // init handle from Device
  void InitHandle();

protected:
  virtual canscope::ValueMapDevicePropertyStore* DevicePrefs() = 0;

private:
  virtual void Observe(int type, 
                       const common::NotificationSource& source, 
                       const common::NotificationDetails& details);

  common::NotificationRegistrar registrar_;

  DeviceBase* device_;
  int current_pref_;
  bool batch_mode_;
};
