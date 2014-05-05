#pragma once

#include "base/values.h"

#include "device/property/store_member.h"
#include "device/property/enum_store_member.h"
#include "device/property/value_map_device_property_store.h"

namespace canscope {

enum Chnl {
  CAN_L,  // CAN-L hardware index is 0
  CAN_H,
  CAN_DIFF,
  kChnlSize,
};
// VoltRange use to index relate data, so be careful
enum VoltRange {
  k1V,
  k2V,
  k4V,
  k8V,
  k20V,
  k50V,
  kVoltRangeSize,
};

// unit V
double Volt(VoltRange range);

// Volt has 8 div
const int kVoltDivNum = 8;

enum TimeBase {
  k1us,
  k2us,
  k5us,
  k10us,
  k20us,
  k50us,
  k100us,
  k200us,
  k500us,
  k1ms,
  k2ms,
  k5ms,
  k10ms,
  k20ms,
  k50ms,
  k100ms,
  k200ms,
  k500ms,
  k1s,
  kTimeBaseSize,
};
// unit ns
uint32 TimeBaseValue(TimeBase time_base);

enum Coupling {
  kDC,
  kAC,
};

enum DiffCtrl {
  kSub,       // CAN-H - CAN-L
  kAverage,   // (CAN-H + CAN-L)/2
};

enum TriggerSource {
  kTriggerSourceCANH,
  kTriggerSourceCANL,
  kTriggerSourceCANDIFF,
  kTriggerSourceCANRxD,
  kTriggerSourceCANTxD,
  kTriggerSourceFrameStart,
  kTriggerSourceExernal,
};

bool IsTriggerSourceChnl(TriggerSource trigger_source);
Chnl TriggerSource2Chnl(TriggerSource trigger_source);
TriggerSource Chnl2TriggerSource(Chnl chnl);

enum TriggerType {
  kRisingEdge, 
  kFallingEdge, 
  kDoubleEdge,
  kPositivePulse,
  kNegativePulse,
};

enum TriggerMode {
  kAuto,
  kNormal,
};

enum TriggerSens {
  kDefault,
  kStrengthen,
};

enum CompareType {
  kGreater,
  kLess,
  kEqual,
};

DECLARE_ENUM_STORE_MEMBER(VoltRange);
DECLARE_ENUM_STORE_MEMBER(TimeBase);
DECLARE_ENUM_STORE_MEMBER(Coupling);
DECLARE_ENUM_STORE_MEMBER(DiffCtrl);
DECLARE_ENUM_STORE_MEMBER(TriggerSource);
DECLARE_ENUM_STORE_MEMBER(TriggerType);
DECLARE_ENUM_STORE_MEMBER(TriggerMode);
DECLARE_ENUM_STORE_MEMBER(TriggerSens);
DECLARE_ENUM_STORE_MEMBER(CompareType);

class OscDeviceProperty {
public:
  OscDeviceProperty() {}
  virtual ~OscDeviceProperty() {}

  // take owned of dict, and use it to init the prefs_
  void Init(base::Value* dict);
  // Init for prefs_ be already init
  void Init();

  void DetachFromThread();

  ::device::ValueMapDevicePropertyStore prefs_;

  // CAN-H
  VoltRangeStoreMember range_can_h;
  ::device::DoubleStoreMember offset_can_h;
  CouplingStoreMember coupling_can_h;

  // CAN-L
  VoltRangeStoreMember range_can_l;
  ::device::DoubleStoreMember offset_can_l;
  CouplingStoreMember coupling_can_l;

  // CAN-DIFF
  VoltRangeStoreMember range_can_diff;
  ::device::DoubleStoreMember offset_can_diff;

  DiffCtrlStoreMember diff_ctrl;

  // time
  TimeBaseStoreMember time_base;
  ::device::DoubleStoreMember time_offset;

  // unit ms
  ::device::DoubleStoreMember auto_time;

  // trigger
  TriggerSourceStoreMember trigger_source;
  TriggerTypeStoreMember trigger_type;
  TriggerModeStoreMember trigger_mode;
  TriggerSensStoreMember trigger_sens;
  CompareTypeStoreMember compare;
  ::device::DoubleStoreMember trigger_volt;
  // unit ns
  ::device::DoubleStoreMember time_param;

protected:

private:
  DISALLOW_COPY_AND_ASSIGN(OscDeviceProperty);
};
} // namespace canscope
