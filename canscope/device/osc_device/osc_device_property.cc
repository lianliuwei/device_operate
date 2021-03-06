#include "canscope/device/osc_device/osc_device_property.h"

#include "canscope/device/canscope_device_property_constants.h"

namespace {
// map to time_base
static const uint32 kTimeBaseValue[] = {
  1000,
  2000,
  5000,
  10000,
  20000,
  50000,
  100000,
  200000,
  500000,
  1000000,
  2000000,
  5000000,
  10000000,
  20000000,
  50000000,
  100000000,
  200000000,
  500000000,
  1000000000,
};

}
namespace canscope {

IMPLEMENT_ENUM_STORE_MEMBER_INT(VoltRange);
IMPLEMENT_ENUM_STORE_MEMBER_INT(TimeBase);
IMPLEMENT_ENUM_STORE_MEMBER_INT(Coupling);
IMPLEMENT_ENUM_STORE_MEMBER_INT(DiffCtrl);
IMPLEMENT_ENUM_STORE_MEMBER_INT(TriggerSource);
IMPLEMENT_ENUM_STORE_MEMBER_INT(TriggerType);
IMPLEMENT_ENUM_STORE_MEMBER_INT(TriggerMode);
IMPLEMENT_ENUM_STORE_MEMBER_INT(TriggerSens);
IMPLEMENT_ENUM_STORE_MEMBER_INT(CompareType);

double Volt(VoltRange range) {
  switch (range) {
  case k1V: return 1; break;
  case k2V: return 2; break;
  case k4V: return 4; break;
  case k8V: return 8; break;
  case k20V: return 20; break;
  case k50V: return 50; break;
  default: NOTREACHED(); return 0; break;
  }
}

uint32 TimeBaseValue(TimeBase time_base) {
  return kTimeBaseValue[time_base];
}

bool IsTriggerSourceChnl(TriggerSource trigger_source) {
  return trigger_source == kTriggerSourceCANH 
      || trigger_source == kTriggerSourceCANL 
      || trigger_source == kTriggerSourceCANDIFF;
}

Chnl TriggerSource2Chnl(TriggerSource trigger_source) {
  switch (trigger_source) {
  case kTriggerSourceCANH: return CAN_H;
  case kTriggerSourceCANL: return CAN_L;
  case kTriggerSourceCANDIFF: return CAN_DIFF;
  default: NOTREACHED(); return CAN_H;
  }
}

TriggerSource Chnl2TriggerSource(Chnl chnl) {
  switch (chnl) {
  case CAN_H: return kTriggerSourceCANH;
  case CAN_L: return kTriggerSourceCANL;
  case CAN_DIFF: return kTriggerSourceCANDIFF;
  default: NOTREACHED(); return kTriggerSourceCANH;
  }
}

void OscDeviceProperty::Init(base::Value* dict) {
  DCHECK(dict->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  dict->GetAsDictionary(&dict_value);
  prefs_.Reset(dict_value);
  Init();
}

void OscDeviceProperty::Init() {
  range_can_h.Init(kOscCANHVoltRange, &prefs_);
  offset_can_h.Init(kOscCANHOffset, &prefs_);
  coupling_can_h.Init(kOscCANHCoupling, &prefs_);
  range_can_l.Init(kOscCANLVoltRange, &prefs_);
  offset_can_l.Init(kOscCANLOffset, &prefs_);
  coupling_can_l.Init(kOscCANLCoupling, &prefs_);
  range_can_diff.Init(kOscCANDIFFVoltRange, &prefs_);
  offset_can_diff.Init(kOscCANDIFFOffset, &prefs_);
  diff_ctrl.Init(kOscDiffCtrl, &prefs_);
  time_base.Init(kOscTimeBase, &prefs_);
  time_offset.Init(kOscTimeOffset, &prefs_);
  auto_time.Init(kOscAutoTime, &prefs_);
  trigger_source.Init(kOscTriggerSource, &prefs_);
  trigger_type.Init(kOscTriggerType, &prefs_);
  trigger_mode.Init(kOscTriggerMode, &prefs_);
  trigger_sens.Init(kOscTriggerSens, &prefs_);
  compare.Init(kOscCompare, &prefs_);
  trigger_volt.Init(kOscTriggerVolt, &prefs_);
  time_param.Init(kOscTimeParam, &prefs_);
}

void OscDeviceProperty::DetachFromThread() {
  prefs_.DetachFromThread();

  range_can_h.DetachFromThread();
  offset_can_h.DetachFromThread();
  coupling_can_h.DetachFromThread();
  range_can_l.DetachFromThread();
  offset_can_l.DetachFromThread();
  coupling_can_l.DetachFromThread();
  range_can_diff.DetachFromThread();
  offset_can_diff.DetachFromThread();
  diff_ctrl.DetachFromThread();
  time_base.DetachFromThread();
  time_offset.DetachFromThread();
  auto_time.DetachFromThread();
  trigger_source.DetachFromThread();
  trigger_type.DetachFromThread();
  trigger_mode.DetachFromThread();
  trigger_sens.DetachFromThread();
  compare.DetachFromThread();
  trigger_volt.DetachFromThread();
  time_param.DetachFromThread();
}

} // namespace canscope
