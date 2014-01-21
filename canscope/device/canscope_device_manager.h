#pragma once

#include "base/memory/scoped_ptr.h"

#include "canscope/device/device_manager.h"
#include "canscope/device/osc_device.h"
#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/config_manager.h"
#include "canscope/device/canscope_device_runner.h"

namespace base {
class MessageLoopProxy;
}

namespace canscope {

class CANScopeDeviceManager : public DeviceManager {
public:

  // no thread safety, call on Device Thread only.
  static CANScopeDeviceManager* Create();

  OscDevice* osc_device() {
    return &osc_device_;
  }

  ConfigManager* osc_device_config_test() { return &osc_device_config_; }

  // take ownership of value
  void Init(base::Value* value);

  // take ownership of value
  void LoadConfig(base::Value* value);

  // pass out ownership value
  base::DictionaryValue* SaveConfig();

  CANScopeRunner* runner();

private:
  // DeviceManager
  virtual void DestroyImpl() OVERRIDE;
  virtual void DeleteDeviceImpl() OVERRIDE;

  CANScopeDeviceManager(base::MessageLoopProxy* device_loop);
  virtual ~CANScopeDeviceManager() {}

  UsbPortDeviceDelegate device_delegate_;

  ConfigManager osc_device_config_;
  OscDevice osc_device_;

  CANScopeRunner runner_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeDeviceManager);
};

}
