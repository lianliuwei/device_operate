#include "canscope/device/register/sja1000_register.h"

#include "canscope/device/register/canscope_device_register_constants.h"

using namespace device;

namespace canscope {

SJA1000Register::SJA1000Register()
    : memory(kSJA1000Addr, kSJA1000Size)
    , sja_btr(memory.PtrByRelative(kSJABtrOffset), false)
    , slient(memory.PtrByRelative(kSJASlientOffset), 0, true)
    , send_state(memory.PtrByRelative(kSJASendStateOffset), false) 
    , s_alc(memory.PtrByRelative(kSJASAlcOffset)) 
    , s_ecc(memory.PtrByRelative(kSJASEccOffset))
    , int_send(memory.PtrByRelative(kSJAIntOffset), 1, true)
    , int_alc(memory.PtrByRelative(kSJAIntOffset), 6, true)
    , int_bus(memory.PtrByRelative(kSJAIntOffset), 7, true)
    , cfg(memory.PtrByRelative(kSJAStateOffset), 6, false)
    , sending(memory.PtrByRelative(kSJAStateOffset), 7, true) {

}

FrameData SJA1000Register::frame_data() const {
  FrameData data;
  memcpy(&(data.data), 
      const_cast<RegisterMemory&>(memory).PtrByRelative(kSJAFrameDataOffset), 
      sizeof(data.data));
  return data;
}

void SJA1000Register::set_frame_data(const FrameData& data) {
  memcpy(memory.PtrByRelative(kSJAFrameDataOffset), &(data.data), sizeof(data.data));
}

} // namespace canscope


