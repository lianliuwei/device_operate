#include "canscope/device/frame_device/frame_device.h"

#include "canscope/device/frame_device/sja_btr_util.h"

namespace {
int KValidSampleRates[] = {
  100000000,50000000,20000000,
  10000000,5000000,2000000,
  1000000,500000,200000,
  100000,50000,20000,
  10000,5000,2000,
  1000,500,200,
  100,50,20,
  10,5,2,
  1
};

int FrameSampleRate(int baud_rate, int bit_sample_rate) {
  int frame_rate = baud_rate * bit_sample_rate;
  int best_frame_rate = KValidSampleRates[0];
  for (int i = 0; i < arraysize(KValidSampleRates); ++i) {
    if (abs(KValidSampleRates[i] - frame_rate) < abs(best_frame_rate - frame_rate)) {
      best_frame_rate = KValidSampleRates[i];
    }
  }
  return best_frame_rate;
}

static const int kMemorySizeRate = 0x10; // no idea
}

namespace canscope {

int FrameDevice::GetFrameStorageSize() {
  double percent = frame_storage_percent.value();
  CHECK(percent >= 0 && percent <= 100.0) << "percent is 0 - 100";
  return DeviceStorageSize(device_type_) * percent / 100;
}
  
void FrameDevice::SetSJA1000() {
  sja1000_.slient.set_value(!ack_enable.value());
  sja1000_.sja_btr.set_value(sja_btr.value());
}

void FrameDevice::SetFrameStorage() {
  uint32 frame_depth = GetFrameStorageSize() / kFrameSize * kFrameSize;
  frame_storage_.frame_depth.set_value(frame_depth);
}

void FrameDevice::SetWaveStorage() {
  int baud_rate = BaudRate();
  int frame_sample_rate = FrameSampleRate(baud_rate, bit_sample_rate.value());
  uint32 frame_len = (frame_bit.value() * 2 * 1.0 * frame_sample_rate / baud_rate);
  frame_len = frame_len & 0xFFFFFFF8;
  wave_storage_.frame_len.set_value(frame_len);
 
  int frame_storage_size = GetFrameStorageSize();
  uint32 wave_start = frame_storage_size & 0xFFFFFFF8;
  wave_storage_.wave_start.set_value(wave_start);

  int memory_size = DeviceStorageSize(device_type_) / kMemorySizeRate;
  uint32 wave_end = memory_size - (memory_size - wave_start) % frame_len;
  wave_storage_.wave_end.set_value(wave_end);

  uint32 div_coe = kMaxBaudRate / frame_sample_rate -1;
  wave_storage_.div_coe.set_value(div_coe);

}

void FrameDevice::SetSoftDiff() {
  soft_diff_->sja_btr.set_value(sja_btr.value());
  soft_diff_->sys_cfg.set_value(device_enable.value()); 
  int baud_rate = SJABtrToBaudRate(sja_btr.value());
  soft_diff_->fil_div.set_value(BaudRateToFilDiv(baud_rate));
}

#define CHECK_DEVICE(error) \
do { \
  if ((error) != canscope::device::OK) { \
    return; \
  } \
} while (0)


void FrameDevice::SetAll() {
  SetSJA1000();
  SetFrameStorage();
  SetWaveStorage();
  SetSoftDiff();
  device::Error err;
   err = WriteDeviceRange(sja1000_.memory, sja1000_.SysOffset(), sja1000_.SysSize());
   CHECK_DEVICE(err);
  err = WriteDevice(frame_storage_.memory);
  CHECK_DEVICE(err);
   err = WriteDevice(wave_storage_.memory);
   CHECK_DEVICE(err);
   err = WriteDeviceRange(soft_diff_->memory, soft_diff_->FilDivOffset(), soft_diff_->FilDivSize());
   CHECK_DEVICE(err);
   // set sys final after config all.
   err = WriteDeviceRange(soft_diff_->memory, soft_diff_->SysOffset(), soft_diff_->SysSize());
  CHECK_DEVICE(err);

}

#undef CHECK_DEVICE

FrameDevice::FrameDevice(DeviceDelegate* device_delegate, 
                         ConfigManager* config_manager,
                         SoftDiffRegister* soft_diff)
    : DeviceBase(config_manager)
    , device_delegate_(device_delegate)
    , soft_diff_(soft_diff) {
  DCHECK(soft_diff);
}

device::Error FrameDevice::WriteDevice(::device::RegisterMemory& memory) {
  return device_delegate_->WriteDevice(
      memory.start_addr(), memory.buffer(), memory.size());
}

device::Error FrameDevice::ReadDevice(::device::RegisterMemory& memory) {
  return device_delegate_->ReadDevice(
      memory.start_addr(), memory.buffer(), memory.size());
}

device::Error FrameDevice::WriteDeviceRange(::device::RegisterMemory& memory, 
                                            int start_offset, 
                                            int size) {
  DCHECK(start_offset + size <= memory.size());
  return device_delegate_->WriteDevice(
      memory.start_addr(), memory.PtrByRelative(start_offset), size);
}

device::Error FrameDevice::ReadDeviceRange(::device::RegisterMemory& memory, 
                                           int start_offset, 
                                           int size) {
  DCHECK(start_offset + size <= memory.size());
  return device_delegate_->ReadDevice(
      memory.start_addr(), memory.PtrByRelative(start_offset), size);
}

} // namespace canscope
