#pragma once

#include "canscope/device/register/soft_diff_register.h"
#include "canscope/device/register/analog_ctrl_register.h"
#include "canscope/device/register/analog_switch_register.h"
#include "canscope/device/register/trigger1_register.h"
#include "canscope/device/register/trigger2_register.h"

namespace canscope {
enum DeviceType {
  DT_CS1201,
  DT_CS1202,
  DT_CS1203,
};
// X K point of Device
int DeviceK(DeviceType device_type);

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

struct CalibrateInfo {
  uint32 N1;
  double Kcal;
  double VCal_base;

  // V offset after calibrate
  double Voffset(double offset);

  // set to hardware
  uint32 Gain();
  uint32 CH_OFFSET(double offset);
};

struct ChnlConfig {
  VoltRange range;
  double offset;
  Coupling coupling;
};

class OscDevice {
public:
  CalibrateInfo GetCalibrateInfo(Chnl chnl, VoltRange range);
  ChnlConfig GetChnlConfig(Chnl chnl);

  void SetVoltBase(Chnl chnl);
  void SetVoltOffset(Chnl chnl);

  // set register according to property
  void SetAnalogCtrl(Chnl chnl);
  void SetSoftDiff(Chnl chnl);
  void SetAnalogSwitch(Chnl chnl);
  void SetTrigger1(DeviceType device_type);
  void SetTrigger2();
  void SetEye();

  void TriggerVolt(uint8* cmp_high, uint8* cmp_low);

  // Osc Device Property
  // volt
  ChnlConfig chnl_configs[kChnlSize];
  DiffCtrl diff_ctrl;
  // time
  TimeBase time_base;
  double time_offset;
  // unit ms
  double auto_time;
  // trigger
  TriggerSource trigger_source;
  TriggerType trigger_type;
  TriggerMode trigger_mode;
  TriggerSens trigger_sens;
  CompareType compare;
  double trigger_volt;
  // unit ns
  double time_param;

private:
  AnalogCtrlRegister analog_ctrl_;
  SoftDiffRegister soft_diff_;
  AnalogSwitchRegister analog_switch_;
  Trigger1Register trigger1_;
  Trigger2Register trigger2_;

  DISALLOW_COPY_AND_ASSIGN(OscDevice);
};
} // namespace canscope
