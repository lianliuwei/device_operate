#pragma once

#include "base/values.h"

#include "device/property/store_member.h"
#include "device/property/value_map_device_property_store.h"

namespace device {
typedef StoreMember<uint16> Uint16StoreMember;

template <>
inline void StoreMember<uint16>::UpdatePref(const uint16& value) {
  prefs()->SetValue(pref_name(), new base::FundamentalValue(value));
}
template <>
inline bool StoreMember<uint16>::UpdateValueInternal(const base::Value& value) const {
  int temp;
  bool ret = value.GetAsInteger(&temp);
  value_ = static_cast<uint16>(temp);
  return ret;
}

} // namespace device

namespace canscope {

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

  ::device::ValueMapDevicePropertyStore prefs_;

  ::device::BooleanStoreMember device_enable;
  ::device::BooleanStoreMember ack_enable;
  ::device::Uint16StoreMember sja_btr;
  ::device::IntegerStoreMember bit_sample_rate;
  ::device::IntegerStoreMember frame_bit;
  ::device::DoubleStoreMember frame_storage_percent;
  
private:
  DISALLOW_COPY_AND_ASSIGN(FrameDeviceProperty);
};

} // namespace canscope
