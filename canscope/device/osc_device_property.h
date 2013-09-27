#pragma once

#include "base/values.h"

#include "canscope/device/property/enum_store_member.h"
#include "canscope/device/property/value_map_device_property_store.h"

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
  kVoltBaseSize,
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
// unit ms
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

DECLARE_ENUM_STORE_MEMBER(Chnl);
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
  void Init(base::DictionaryValue* dict);
  // Init for prefs_ be already init
  void Init();

  ValueMapDevicePropertyStore prefs_;

  // CAN-H
  VoltRangeStoreMember range_can_h;
  DoubleStoreMember offset_can_h;
  CouplingStoreMember coupling_can_h;

  // CAN-L
  VoltRangeStoreMember range_can_l;
  DoubleStoreMember offset_can_l;
  CouplingStoreMember coupling_can_l;

  // CAN-DIFF
  VoltRangeStoreMember range_can_diff;
  DoubleStoreMember offset_can_diff;

  DiffCtrlStoreMember diff_ctrl;

  // time
  TimeBaseStoreMember time_base;
  DoubleStoreMember time_offset;

  // unit ms
  DoubleStoreMember auto_time;

  // trigger
  TriggerSourceStoreMember trigger_source;
  TriggerTypeStoreMember trigger_type;
  TriggerModeStoreMember trigger_mode;
  TriggerSensStoreMember trigger_sens;
  CompareTypeStoreMember compare;
  DoubleStoreMember trigger_volt;
  // unit ns
  DoubleStoreMember time_param;

protected:

private:
  DISALLOW_COPY_AND_ASSIGN(OscDeviceProperty);
};
} // namespace canscope
