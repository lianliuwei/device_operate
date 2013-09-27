#include "canscope/device/osc_device_handle.h"

#include "base/bind.h"

using namespace base;

namespace canscope {

OscDeviceHandle::OscDeviceHandle(OscDevice* device)
    : DeviceHandleBase(device)
    , device_(device)
    , volt_range_can_h(this,
        properties_.range_can_h, device->range_can_h,
        Bind(&OscDevice::SetVoltRange, Unretained(device), CAN_H))
    , offset_can_h(this,
        properties_.offset_can_h, device->offset_can_h,
        Bind(&OscDevice::SetVoltOffset, Unretained(device), CAN_H))
    , coupling_can_h(this,
        properties_.coupling_can_h, device->coupling_can_h,
        Bind(&OscDevice::SetCoupling, Unretained(device), CAN_H))

    , volt_range_can_l(this,
        properties_.range_can_l, device->range_can_l,
        Bind(&OscDevice::SetVoltRange, Unretained(device), CAN_L))
    , offset_can_l(this,
        properties_.offset_can_l, device->offset_can_l,
        Bind(&OscDevice::SetVoltOffset, Unretained(device), CAN_L))
    , coupling_can_l(this,
        properties_.coupling_can_l, device->coupling_can_l,
        Bind(&OscDevice::SetCoupling, Unretained(device), CAN_L))

    , volt_range_can_diff(this,
        properties_.range_can_diff, device->range_can_diff,
        Bind(&OscDevice::SetVoltRange, Unretained(device), CAN_DIFF))
    , offset_can_diff(this,
        properties_.offset_can_diff, device->offset_can_diff,
        Bind(&OscDevice::SetVoltOffset, Unretained(device), CAN_DIFF))
    
    , diff_ctrl(this,
        properties_.diff_ctrl, device->diff_ctrl,
        Bind(&OscDevice::SetDiffCtrl, Unretained(device)))

    , time_base(this,
        properties_.time_base, device->time_base,
        Bind(&OscDevice::SetTimeBase, Unretained(device)))
    , time_offset(this,
        properties_.time_offset, device->time_offset,
        Bind(&OscDevice::SetTimeOffset, Unretained(device)))

    , auto_time(this,
        properties_.auto_time, device->auto_time,
        Bind(&OscDevice::SetAutoTime, Unretained(device)))
    , trigger_source(this,
        properties_.trigger_source, device->trigger_source,
        Bind(&OscDevice::SetTriggerSource, Unretained(device)))
    , trigger_type(this,
        properties_.trigger_type, device->trigger_type,
        Bind(&OscDevice::SetTriggerType, Unretained(device)))
    , trigger_mode(this,
        properties_.trigger_mode, device->trigger_mode,
        Bind(&OscDevice::SetTriggerMode, Unretained(device)))
    , trigger_sens(this,
        properties_.trigger_sens, device->trigger_sens,
        Bind(&OscDevice::SetTriggerSens, Unretained(device)))
    , compare(this,
        properties_.compare, device->compare,
        Bind(&OscDevice::SetCompare, Unretained(device)))
    , trigger_volt(this,
        properties_.trigger_volt, device->trigger_volt,
        Bind(&OscDevice::SetTriggerVolt, Unretained(device)))

    , time_param(this,
        properties_.time_param, device->time_param,
        Bind(&OscDevice::SetTimeParam, Unretained(device))) {
  InitHandle();
  properties_.Init();
}

} // namespace canscope
