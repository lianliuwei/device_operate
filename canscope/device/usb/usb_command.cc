#include "canscope/device/usb/usb_command.h"

#include "canscope/device/usb/usb_constants.h"

namespace canscope {
UsbCommand::UsbCommand()
    : memory(kUsbCommandAddr, kUsbCommandSize + CMD_MAX_LEN)
    , cmd_id(memory.PtrByRelative(kUsbCommandIDOffset), false)
    , mode(memory.PtrByRelative(kUsbCommandModeOffset), 0, true)
    , addr(memory.PtrByRelative(kUsbCommandAddrOffset), false)
    , data_size(memory.PtrByRelative(kUsbCommandDataSizeOffset), false)
    , func_addr(memory.PtrByRelative(kUsbCommandFucAddrOffset), false) {}


uint8* UsbCommand::write_data_ptr() {
  return memory.PtrByRelative(kUsbCommandReadOffset);
}
// static
int UsbCommand::write_data_max_size() {
  return static_cast<int>(CMD_MAX_LEN);
}

uint8* UsbCommand::read_data_ptr() {
  return memory.PtrByRelative(kUsbCommandReadOffset);
}
// static
int UsbCommand::read_data_max_size() {
  return kUsbCommandSize + CMD_MAX_LEN - kUsbCommandReadOffset;
}

int UsbCommand::size() const {
  return memory.size();
}

void UsbCommand::reset() {
  memset(memory.buffer(), 0x0, memory.size());
}


} // namespace canscope