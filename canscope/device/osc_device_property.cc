#include "canscope/device/osc_device_property.h"

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


void OscDeviceProperty::Init() {
  range_can_h.Init(kRangeCANHItem, &prefs_);
  offset_can_h.Init(kOffsetCANHItem, &prefs_);
  coupling_can_h.Init(kCouplingCANHItem, &prefs_);
  range_can_l.Init(kRangeCANLItem, &prefs_);
  offset_can_l.Init(kOffsetCANLItem, &prefs_);
  coupling_can_l.Init(kCouplingCANLItem, &prefs_);
  range_can_diff.Init(kRangeCANDIFFItem, &prefs_);
  offset_can_diff.Init(kOffsetCANDIFFItem, &prefs_);
  diff_ctrl.Init(kDiffCtrlItem, &prefs_);
  time_base.Init(kTimeBaseItem, &prefs_);
  time_offset.Init(kTimeOffsetItem, &prefs_);
  auto_time.Init(kAutoTimeItem, &prefs_);
  trigger_source.Init(kTriggerSourceItem, &prefs_);
  trigger_type.Init(kTriggerTypeItem, &prefs_);
  trigger_mode.Init(kTriggerModeItem, &prefs_);
  trigger_sens.Init(kTriggerSensItem, &prefs_);
  compare.Init(kCompareItem, &prefs_);
  trigger_volt.Init(kTriggerVoltItem, &prefs_);
  time_param.Init(kTimeParamItem, &prefs_);
}

} // namespace canscope
