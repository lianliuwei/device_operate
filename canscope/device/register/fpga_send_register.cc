#include "canscope/device/register/fpga_send_register.h"

#include "canscope/device/register/canscope_device_register_constants.h"

using namespace device;

canscope::FpgaSendRegister::FpgaSendRegister()
    : memory(kFpgaSendAddr, kFpgaSendSize)
    , send_num(memory.PtrByRelative(kSendNumOffset))
    , bit_num(memory.PtrByRelative(kBitNumOffset))
    , btr_div(memory.PtrByRelative(kBtrDivOffset), false)
    , ack(memory.PtrByRelative(kAckOffset), 0,true)
    , ack_bit(memory.PtrByRelative(kAckBitOffset))
    , start_send(memory.PtrByRelative(kSendStateOffset), 0, true)
    , send_state(memory.PtrByRelative(kSendStateOffset), 0, false)
    , set_ack(memory.PtrByRelative(kAckStateOffset), 1, true)
    , ack_state(memory.PtrByRelative(kAckStateOffset), 1, false)
    , dstb_start(memory.PtrByRelative(kDstbStartOffset), false)
    , dstb_end(memory.PtrByRelative(kDstbEndOffset), false)
    , dstb(memory.PtrByRelative(kDstbOffset), 0, true) {

}

FpgaFrameData canscope::FpgaSendRegister::frame_data() const {
  FpgaFrameData frame;
  memcpy(&(frame.data),
     const_cast<RegisterMemory&>(memory).PtrByRelative(kFpgaSendFrameDataOffset),
     sizeof(frame.data));

  return frame;
}

void canscope::FpgaSendRegister::set_frame_data(const FpgaFrameData& data) {
  memcpy(memory.PtrByRelative(kFpgaSendFrameDataOffset), &(data.data), sizeof(data.data));
}

FpgaDstbData canscope::FpgaSendRegister::dstb_data() const {
  FpgaDstbData frame;
  memcpy(&(frame.data),
     const_cast<RegisterMemory&>(memory).PtrByRelative(kDstbDataOffset),
     sizeof(frame.data));

  return frame;
}

void canscope::FpgaSendRegister::set_dstb_data(const FpgaDstbData& data) {
  memcpy(memory.PtrByRelative(kDstbDataOffset), &(data.data), sizeof(data.data));
}

int canscope::FpgaSendRegister::SendOffset() const {
  return 0;
}

int canscope::FpgaSendRegister::SendSize() const {
  return 0x18;
}

int canscope::FpgaSendRegister::StateOffset() const {
  return 0x1C;
}

int canscope::FpgaSendRegister::StateSize() const {
  return 4;
}
