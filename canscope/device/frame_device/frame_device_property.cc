#include "canscope/device/frame_device/frame_device_property.h"

#include "canscope/device/canscope_device_property_constants.h"
#include "canscope/device/canscope_device_constants.h"

#include "canscope/device/frame_device/sja_btr_util.h"

namespace canscope {

void FrameDeviceProperty::Init(base::Value* dict) {
  DCHECK(dict->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  dict->GetAsDictionary(&dict_value);
  prefs_.Reset(dict_value);
  Init();
}

void FrameDeviceProperty::Init() {
  device_enable.Init(kFrameDeviceEnable, &prefs_);
  ack_enable.Init(kFrameAckEnable, &prefs_);
  sja_btr.Init(kFrameSjaBtr, &prefs_);
  frame_storage_percent.Init(kFrameFrameStoragePercent, &prefs_);
  bit_sample_rate.Init(kFrameBitSampleRate, &prefs_);
  frame_bit.Init(kFrameBit, &prefs_);
}

void FrameDeviceProperty::DetachFromThread() {
  prefs_.DetachFromThread();
  device_enable.DetachFromThread();
  ack_enable.DetachFromThread();
  sja_btr.DetachFromThread();
  frame_storage_percent.DetachFromThread();
  bit_sample_rate.DetachFromThread();
  frame_bit.DetachFromThread();
}

int FrameDeviceProperty::BaudRate() {
  return SJABtrToBaudRate(sja_btr.value());
}

uint16 FrameDeviceProperty::BtrDiv() {
  return kMaxBaudRate / BaudRate() - 1;
}

} // namespace canscope
