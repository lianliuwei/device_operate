#pragma once

#include "base/values.h"

#include "canscope/device/property/store_member.h"
#include "canscope/device/property/value_map_device_property_store.h"

namespace canscope {

template <>
void StoreMember<uint16>::UpdatePref(const uint16& value);
template <>
bool StoreMember<uint16>::UpdateValueInternal(const base::Value& value) const;

typedef StoreMember<uint16> Uint16StoreMember;

class FrameDeviceProperty {
public:
  FrameDeviceProperty() {}
  virtual ~FrameDeviceProperty() {}

  // take owned of dict, and use it to init the prefs_
  void Init(base::Value* dict);
  // Init for prefs_ be already init
  void Init();
  void DetachFromThread();

  int BaudRate();
  uint16 BtrDiv();

  ValueMapDevicePropertyStore prefs_;

  BooleanStoreMember device_enable;
  BooleanStoreMember ack_enable;
  Uint16StoreMember sja_btr;
  IntegerStoreMember bit_sample_rate;
  IntegerStoreMember frame_bit;
  DoubleStoreMember frame_storage_percent;
  
private:
  DISALLOW_COPY_AND_ASSIGN(FrameDeviceProperty);
};

} // namespace canscope
