#pragma once

#include "canscope/device/frame_device/frame_device_property.h"
#include "canscope/device/device_delegate.h"
#include "canscope/device/device_base.h"
#include "canscope/device_errors.h"
#include "canscope/device/canscope_device_constants.h"
#include "canscope/device/register/sja1000_register.h"
#include "canscope/device/register/frame_storage_register.h"
#include "canscope/device/register/wave_storage_register.h"
#include "canscope/device/register/soft_diff_register.h"

namespace canscope {

class FrameDeviceHandle;

class FrameDevice : public FrameDeviceProperty
                  , public DeviceBase {
public:
  // need soft_diff from OscDevice
  FrameDevice(DeviceDelegate* device_delegate, 
              ConfigManager* config_manager, 
              SoftDiffRegister* soft_diff);
  virtual ~FrameDevice() {}

  void ConfigUpdate();

  void set_device_type(canscope::DeviceType value) { device_type_ = value; }
  canscope::DeviceType device_type() const { return device_type_; }

  // property result in set register to hardware
  void SetAll();
 
private:
  void SetSJA1000();
  void SetFrameStorage();
  void SetWaveStorage();
  void SetSoftDiff();

  int GetFrameStorageSize();

  // implement DeviceBase
  virtual canscope::ValueMapDevicePropertyStore* DevicePrefs() { return &prefs_; }

  device::Error WriteDevice(::device::RegisterMemory& memory);
  device::Error ReadDevice(::device::RegisterMemory& memory);
  device::Error WriteDeviceRange(::device::RegisterMemory& memory, int start_offset, int size);
  device::Error ReadDeviceRange(::device::RegisterMemory& memory, int start_offset, int size);

  friend class FrameDeviceHandle;

  SJA1000Register sja1000_;
  FrameStorageRegister frame_storage_;
  WaveStorageRegister wave_storage_;
  SoftDiffRegister* soft_diff_; // need sys_cfg sja_btr fil_div
  DeviceType device_type_;

  DeviceDelegate* device_delegate_;

  DISALLOW_COPY_AND_ASSIGN(FrameDevice);
};

} // namespace canscope
