#include "canscope/device/test/device_util.h"

#include "base/logging.h"

using namespace canscope::device;

namespace canscope {

Error WriteDevice(DeviceDelegate* device_delegate, 
                  ::device::RegisterMemory& memory) {
  return device_delegate->WriteDevice(
      memory.start_addr(), memory.buffer(), memory.size());
}

Error ReadDevice(DeviceDelegate* device_delegate, 
                 ::device::RegisterMemory& memory) {
  return device_delegate->ReadDevice(
      memory.start_addr(), memory.buffer(), memory.size());
}

Error WriteDeviceRange(DeviceDelegate* device_delegate, 
                       ::device::RegisterMemory& memory, 
                       int start_offset, 
                       int size) {
  DCHECK(start_offset + size <= memory.size());
  return device_delegate->WriteDevice(
      memory.start_addr() + start_offset, memory.PtrByRelative(start_offset), size);
}

Error ReadDeviceRange(DeviceDelegate* device_delegate, 
                      ::device::RegisterMemory& memory, 
                      int start_offset, 
                      int size) {
  DCHECK(start_offset + size <= memory.size());
  return device_delegate->ReadDevice(
      memory.start_addr() + start_offset, memory.PtrByRelative(start_offset), size);
}

} // namespace canscope