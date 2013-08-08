#include "canscope/device/osc_device.h"

#include "canscope/device/register/scope_ctrl_register.h"

#include <cmath>

using namespace device;

namespace {
// match to VoltRange
static const uint8 kSensCoeff[] = {4,8,16,32,80,200};

static const double OP07 = -1.96;

// march VlotRange
static const canscope::CalibrateInfo kDefaultCalibrateInfo[] = {
  {0x0000D164, OP07/0.2, -0.102040816,},
  {0x000092CD, OP07/0.2, -0.204081633,},
  {0x00005436, OP07/0.2, -0.408163265,},
  {0x0000E58B, OP07/0.02, -0.081632653,},
  {0x000092CD, OP07/0.02, -0.204081633,},
  {0x00004010, OP07/0.02, -0.510204082,},
};


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

// map to time_base
static const uint32 kCOE[] = {
  0,
  1,
  4,
  9,
  19,
  49,
  99,
  199,
  499,
  999,
  1999,
  4999,
  9999,
  19999,
  49999,
  99999,
  199999,
  499999,
  1999999,  
};

}

namespace canscope {

uint16 CH_ZERO(VoltRange range, double offset) {
  return static_cast<uint16>((offset * 250.0)/ Volt(range) + 127);
}
bool Attenuation(VoltRange range) {
  return range >= k8V;
}

bool GetDiffCtrl(DiffCtrl diff_ctrl) {
  return diff_ctrl == kAverage;
}
bool GetCoupling(Coupling coupling) {
  return coupling == kDC;
}

uint32 SAMET(uint32 coe) {
  return (coe + 1) * 10;
}

uint32 GetTrigMode(TriggerMode trigger_mode) {
  return trigger_mode == kAuto ? 1 : 0;
}

uint8 CMP_LOW(VoltRange range, double trig_volt, double offset_volt) {
  double div_volt = Volt(range)/kVoltDivNum;
  return static_cast<uint8>((trig_volt + offset_volt)/div_volt * 31.25 + 127);
}

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

int DeviceK(DeviceType device_type) {
  return device_type == DT_CS1203 ? 8 : 2;
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

double CalibrateInfo::Voffset(double offset) {
  return VCal_base + (offset) / Kcal;
}

uint32 CalibrateInfo::CH_OFFSET(double offset) {
  return static_cast<uint32>(
      (Voffset(offset) + 2.67) * pow(2.0, 16) / 4.2 / 1.25);
}

OscDevice::OscDevice(DeviceDelegate* device_delegate)
    : device_delegate_(device_delegate) {}


CalibrateInfo OscDevice::GetCalibrateInfo(Chnl chnl, VoltRange range) {
  // TODO get real Calibrate Info from hardware
  return kDefaultCalibrateInfo[range];
}

ChnlConfig OscDevice::GetChnlConfig(Chnl chnl) {
  return chnl_configs[chnl];
}

void OscDevice::SetAnalogCtrl(Chnl chnl) {
  ChnlConfig config = GetChnlConfig(chnl);
  CalibrateInfo info = GetCalibrateInfo(chnl, config.range);
  if (chnl == CAN_H) {
    analog_ctrl_.can_h_gain.set_value(info.Gain());
    analog_ctrl_.can_h_offset.set_value(info.CH_OFFSET(config.offset));
  } else if (chnl == CAN_L) {
    analog_ctrl_.can_l_gain.set_value(info.Gain());
    analog_ctrl_.can_l_offset.set_value(info.CH_OFFSET(config.offset));
  } else {
    NOTREACHED();
  }
}

void OscDevice::SetSoftDiff(Chnl chnl) {
  ChnlConfig config = GetChnlConfig(chnl);
  CalibrateInfo info = GetCalibrateInfo(chnl, config.range);
  if (chnl == CAN_H) {
    soft_diff_.ch_sens_canh.set_value(kSensCoeff[config.range]);
    soft_diff_.ch_zero_canh.set_value(CH_ZERO(config.range, config.offset));
  } else if (chnl == CAN_L) {
    soft_diff_.ch_sens_canl.set_value(kSensCoeff[config.range]);
    soft_diff_.ch_zero_canl.set_value(CH_ZERO(config.range, config.offset));
  } else if (chnl == CAN_DIFF) {
    soft_diff_.ch_sens_candiff.set_value(kSensCoeff[config.range]);
    soft_diff_.ch_zero_candiff.set_value(CH_ZERO(config.range, config.offset));
  } else {
    NOTREACHED();
  }
  soft_diff_.filtering.set_value(5);
  soft_diff_.diff_ctrl.set_value(GetDiffCtrl(diff_ctrl));
}

void OscDevice::SetAnalogSwitch(Chnl chnl) {
  ChnlConfig config = GetChnlConfig(chnl);
  CalibrateInfo info = GetCalibrateInfo(chnl, config.range);
  if (chnl == CAN_H) {
    analog_switch_.coupling_canh.set_value(GetCoupling(config.coupling));
    analog_switch_.attenuation_canh.set_value(Attenuation(config.range));
  } else if (chnl == CAN_L) {
    analog_switch_.coupling_canl.set_value(GetCoupling(config.coupling));
    analog_switch_.attenuation_canl.set_value(Attenuation(config.range));
  } else {
    NOTREACHED();
  }
}

void OscDevice::SetTrigger1(DeviceType device_type) {
  trigger1_.div_coe.set_value(kCOE[time_base]);
  uint32 base_value = kTimeBaseValue[time_base];
  uint32 samet = SAMET(trigger1_.div_coe.value());
  uint32 temp;
  uint32 k = DeviceK(device_type);
  temp = static_cast<uint32>(((time_base*5*k + time_offset) /samet) * 2);
  temp &= 0xFFFFFFFC; // div by 4
  trigger1_.trig_pre.set_value(temp);
  temp = std::max(8U, 
      static_cast<uint32>(((time_base*5*k - time_offset)/samet) * 2));
  temp &= 0xFFFFFFFC; // div by 4
  trigger1_.trig_post.set_value(temp);
  trigger1_.auto_time.set_value(static_cast<uint32>(auto_time * 1000000 / 10));
}

void OscDevice::SetTrigger2() {
  trigger2_.trig_source.set_value(trigger_source);
  trigger2_.trig_type.set_value(trigger_type);
  trigger2_.trig_mode.set_value(GetTrigMode(trigger_mode));
  trigger2_.compare.set_value(compare);
  trigger2_.trig_time.set_value((static_cast<uint32>(time_param)/10 - 1) 
      & 0x3FFFFFFF);
  
  if (!IsTriggerSourceChnl(trigger_source)) {
    return;
  }
  //CMP_VOL
  uint8 cmp_high;
  uint8 cmp_low;
  TriggerVolt(&cmp_high, &cmp_low);
  trigger2_.cmp_high.set_value(cmp_high);
  trigger2_.cmp_low.set_value(cmp_low);

  // TODO set eye center
}

void OscDevice::TriggerVolt(uint8* cmp_high, uint8* cmp_low) {
  uint8 high = 0x00;
  uint8 low = 0x00;
  ChnlConfig config = GetChnlConfig(TriggerSource2Chnl(trigger_source));
  low = CMP_LOW(config.range, trigger_volt, config.offset);

  if (trigger_sens == kDefault) {
    high = low + 3;
  } else if (trigger_sens == kStrengthen) {
    high = low + 15;
  }

  if (low > 250) {
    low = 247;
  }
  if (high > 250) {
    high = 250;
  }
  if (cmp_high)
    *cmp_high = high;
  if (cmp_low)
    *cmp_low = low;
}

void OscDevice::WriteDevice(RegisterMemory* memory, bool* state) {
  DCHECK(state);
  DCHECK(memory);
  if (state == false || device_delegate_ == NULL)
    return;
  *state = device_delegate_->WriteDevice(
        memory->start_addr(), memory->buffer(), memory->size());
}

void OscDevice::ReadDevice(RegisterMemory* memory, bool* state) {
  DCHECK(state);
  DCHECK(memory);
  if (state == false || device_delegate_ == NULL)
    return;
  *state = device_delegate_->ReadDevice(
        memory->start_addr(), memory->buffer(), memory->size());
}

void OscDevice::SetVoltRange(Chnl chnl) {
  if (chnl != CAN_DIFF) {
    SetAnalogCtrl(chnl);
    SetAnalogSwitch(chnl);
  }
  SetSoftDiff(chnl);
  SetTrigger2();

  bool state = true;
  if (chnl != CAN_DIFF) {
    WriteDevice(&(analog_ctrl_.memory), &state);
    WriteDevice(&(analog_switch_.memory), &state);
  }
  WriteDevice(&(soft_diff_.memory), &state);
  WriteDevice(&(trigger2_.memory), &state);
}

void OscDevice::SetVoltOffset(Chnl chnl) {
  // same register as SetVoltOffset
  SetVoltRange(chnl);
}

void OscDevice::SetCoupling(Chnl chnl) {
  // CAN-DIFF no Coupling
  DCHECK(chnl != CAN_DIFF);
  SetAnalogSwitch(chnl);
  bool state = true;
  WriteDevice(&(analog_switch_.memory), &state);
}

void OscDevice::SetDiffCtrl() {
  // just set diff_ctrl
  SetSoftDiff(CAN_DIFF);
  bool state = true;
  WriteDevice(&(soft_diff_.memory), &state);
}

void OscDevice::SetTimeBase() {
  // TODO device add Device Type obtain.
  SetTrigger1(DT_CS1202);
  bool state = true;
  WriteDevice(&(trigger1_.memory), &state);
}

void OscDevice::SetTimeOffset() {
  // same register as SetTimeBase()
  SetTimeBase();
}

void OscDevice::SetAutoTime() {
  // same register as SetTimeBase()
  SetTimeBase();
}

void OscDevice::SetTriggerSource() {
  SetTrigger2();
  bool state = true;
  WriteDevice(&(trigger2_.memory), &state);
}

void OscDevice::SetTriggerType() {
  // same register as SetTriggerSource()
  SetTriggerSource();
}

void OscDevice::SetTriggerMode() {
  // same register as SetTriggerSource()
  SetTriggerSource();
}

void OscDevice::SetTriggerSens() {
  // same register as SetTriggerSource()
  SetTriggerSource();
}

void OscDevice::SetCompare() {
  // same register as SetTriggerSource()
  SetTriggerSource();
}

void OscDevice::SetTriggerVolt() {
  // same register as SetTriggerSource()
  SetTriggerSource();
}

void OscDevice::SetTimeParam() {
  // same register as SetTriggerSource()
  SetTriggerSource();
}

void OscDevice::UpdateTriggerState() {
  bool state;
  ReadDevice(&(trigger_state_.memory), &state);
}

void OscDevice::SetAll() {
  SetAnalogCtrl(CAN_H);
  SetAnalogCtrl(CAN_L);
  SetSoftDiff(CAN_H);
  SetSoftDiff(CAN_L);
  SetSoftDiff(CAN_DIFF);
  SetAnalogSwitch(CAN_H);
  SetAnalogSwitch(CAN_L);
  // TODO device add Device Type obtain.
  SetTrigger1(DT_CS1202);
  SetTrigger2();

  bool state;
  WriteDevice(&(analog_ctrl_.memory), &state);
  WriteDevice(&(soft_diff_.memory), &state);
  WriteDevice(&(analog_switch_.memory), &state);
  WriteDevice(&(trigger1_.memory), &state);
  WriteDevice(&(trigger2_.memory), &state);
}

bool OscDevice::Start() {
  ScopeCtrlRegister scope_ctrl;
  scope_ctrl.scope_ctrl.set_value(true);
  bool state;
  WriteDevice(&(scope_ctrl.memory), &state);
  return state;
}

bool OscDevice::IsCollected() {
  return trigger_state_.clet_bit.value();
}

} // namespace canscope
