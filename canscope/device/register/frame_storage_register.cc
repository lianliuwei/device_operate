#include "canscope/device/register/frame_storage_register.h"

#include "canscope/device/register/canscope_device_register_constants.h"

using namespace device;

namespace canscope {

FrameStorageRegister::FrameStorageRegister()
    : memory(kFrameStorageAddr, kFrameStorageSize)
    , frame_depth(memory.PtrByRelative(kFrameDepthOffset), false)
    , frame_num(memory.PtrByRelative(kFrameNumOffset), false)
    , overflow(memory.PtrByRelative(kFrameOverFlowOffset), 0, true) {

}

} // namespace canscope
