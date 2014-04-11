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
#include "canscope/device/canscope_device_constants.h"


class ConfigManager;

namespace canscope {

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

  void set_device_type(canscope::DeviceType value) { device_type_ = value; }
  canscope::DeviceType device_type() const { return device_type_; }

private:
  // implement DeviceBase
  virtual canscope::ValueMapDevicePropertyStore* DevicePrefs() { return &prefs_; }

  // set register according to property
  void SetAnalogCtrl(Chnl chnl);
  void SetSoftDiff(Chnl chnl);
  void SetAnalogSwitch(Chnl chnl);
  void SetTrigger1(DeviceType device_type);
  void SetTrigger2();
  void SetEye();

  void TriggerVolt(uint8* cmp_high, uint8* cmp_low);

  void WriteDevice(::device::RegisterMemory* memory, bool* state);
  void ReadDevice(::device::RegisterMemory* memory, bool* state);

  AnalogCtrlRegister analog_ctrl_;
  SoftDiffRegister soft_diff_;
  AnalogSwitchRegister analog_switch_;
  Trigger1Register trigger1_;
  Trigger2Register trigger2_;
  DeviceType device_type_;

  DeviceDelegate* device_delegate_;

  DISALLOW_COPY_AND_ASSIGN(OscDevice);
};
} // namespace canscope
