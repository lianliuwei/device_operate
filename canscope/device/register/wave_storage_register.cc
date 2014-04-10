#include "canscope/device/register/wave_storage_register.h"

#include "canscope/device/register/canscope_device_register_constants.h"

namespace canscope {

WaveStorageRegister::WaveStorageRegister()
    : memory(kWaveStorageAddr, kWaveStorageSize)
    , frame_len(memory.PtrByRelative(kFrameLenOffset), false)
    , wave_start(memory.PtrByRelative(kWaveStartOffset), false)
    , wave_end(memory.PtrByRelative(kWEaveEndOffset), false)
    , div_coe(memory.PtrByRelative(kDivCoreOffset), false)
    , wave_lost(memory.PtrByRelative(KWaveLostOffset), false) {
}

} // namespace canscope
