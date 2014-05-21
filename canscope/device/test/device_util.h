#pragma once

#include "canscope/device_errors.h"
#include "canscope/device/device_delegate.h"
#include "device/register_memory.h"

namespace canscope {

device::Error WriteDevice(DeviceDelegate* device_delegate, 
                          ::device::RegisterMemory& memory);

device::Error ReadDevice(DeviceDelegate* device_delegate, 
                         ::device::RegisterMemory& memory);

device::Error WriteDeviceRange(DeviceDelegate* device_delegate, 
                               ::device::RegisterMemory& memory, 
                               int start_offset, 
                               int size);

device::Error ReadDeviceRange(DeviceDelegate* device_delegate, 
                              ::device::RegisterMemory& memory, 
                              int start_offset, 
                              int size);

} // namespace canscope