#pragma once

#include "canscope/device/osc_device_property.h"
#include "canscope/device/register/soft_diff_register.h"
#include "canscope/device/register/analog_ctrl_register.h"
#include "canscope/device/register/analog_switch_register.h"
#include "canscope/device/register/trigger1_register.h"
#include "canscope/device/register/trigger2_register.h"
#include "canscope/device/register/trigger_state_register.h"
#include "canscope/device/device_delegate.h"
#include "canscope/device/device_base.h"

class ConfigManager;

namespace canscope {
// TODO need canscope_device.h
enum DeviceType {
  DT_CS1201,
  DT_CS1202,
  DT_CS1203,
};
// X K point of Device
int DeviceK(DeviceType device_type);


struct CalibrateInfo {
  uint32 N1;
  double Kcal;
  double VCal_base;

  // V offset after calibrate
  double Voffset(double offset);

  // set to hardware
  uint32 Gain() { return N1; }
  uint32 CH_OFFSET(double offset);
};

struct ChnlConfig {
  VoltRange range;
  double offset;
  Coupling coupling;
};

class OscDevice : public OscDeviceProperty
                , public DeviceBase {
public:
  OscDevice(DeviceDelegate* device_delegate, ConfigManager* config_manager);
  virtual ~OscDevice() {}

  void ConfigUpdate();

  CalibrateInfo GetCalibrateInfo(Chnl chnl, VoltRange range);
  VoltRange GetChnlVoltRange(Chnl chnl);
  ChnlConfig GetChnlConfig(Chnl chnl);

   // property result in set register to hardware
  void SetVoltRange(Chnl chnl);
  void SetVoltOffset(Chnl chnl);
  void SetCoupling(Chnl chnl);
  void SetDiffCtrl();
  void SetTimeBase();
  void SetTimeOffset();
  void SetAutoTime();
  void SetTriggerSource();
  void SetTriggerType();
  void SetTriggerMode();
  void SetTriggerSens();
  void SetCompare();
  void SetTriggerVolt();
  void SetTimeParam();
  void SetAll();
  
  // update property.
  void UpdateTriggerState();
  
  bool IsCollected();

  // start scope
  bool Start();

private:
  // DeviceBase
  virtual void LockStatusChanged() {}
  virtual canscope::ValueMapDevicePropertyStore* DevicePrefs() { return &prefs_; }

  // set register according to property
  void SetAnalogCtrl(Chnl chnl);
  void SetSoftDiff(Chnl chnl);
  void SetAnalogSwitch(Chnl chnl);
  void SetTrigger1(DeviceType device_type);
  void SetTrigger2();
  void SetEye();

  void TriggerVolt(uint8* cmp_high, uint8* cmp_low);

  void WriteDevice(device::RegisterMemory* memory, bool* state);
  void ReadDevice(device::RegisterMemory* memory, bool* state);

  AnalogCtrlRegister analog_ctrl_;
  SoftDiffRegister soft_diff_;
  AnalogSwitchRegister analog_switch_;
  Trigger1Register trigger1_;
  Trigger2Register trigger2_;
  TriggerStateRegister trigger_state_;

  DeviceDelegate* device_delegate_;

  DISALLOW_COPY_AND_ASSIGN(OscDevice);
};
} // namespace canscope
