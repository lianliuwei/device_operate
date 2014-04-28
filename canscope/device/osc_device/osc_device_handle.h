#pragma once

#include "canscope/device/osc_device/osc_device.h"

#include "canscope/device/property/property.h"
#include "canscope/device/device_handle_base.h"

namespace canscope {
// 
class OscDeviceHandle : public DeviceHandleBase {
public:
  OscDeviceHandle(OscDevice* device);
  virtual ~OscDeviceHandle() {}

  Property<VoltRange> volt_range_can_h;
  Property<double> offset_can_h;
  Property<Coupling> coupling_can_h;

  Property<VoltRange> volt_range_can_l;
  Property<double> offset_can_l;
  Property<Coupling> coupling_can_l;

  Property<VoltRange> volt_range_can_diff;
  Property<double> offset_can_diff;

  Property<DiffCtrl> diff_ctrl;

  Property<TimeBase> time_base;
  Property<double> time_offset;

  Property<double> auto_time;
  Property<TriggerSource> trigger_source;
  Property<TriggerType> trigger_type;
  Property<TriggerMode> trigger_mode;
  Property<TriggerSens> trigger_sens;
  Property<CompareType> compare;
  Property<double> trigger_volt;

  Property<double> time_param;

private:
  // implement DeviceHandleBase
  virtual ::device::ValueMapDevicePropertyStore* DevicePrefs() { 
    return &(properties_.prefs_); 
  }

  OscDevice* device_;

  OscDeviceProperty properties_;
};

} // namespace canscope
