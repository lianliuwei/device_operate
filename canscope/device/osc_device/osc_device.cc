#include "canscope/device/osc_device/osc_device.h"

#include <cmath>

#include "canscope/device/register/scope_ctrl_register.h"
#include "canscope/device_errors.h"
#include "canscope/device/config_manager.h"

using namespace device;
using namespace canscope::device;

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
  return coupling == kAC;
}

uint32 SAMET(uint32 coe) {
  return (coe + 1) * 10;
}

uint32 GetTrigMode(TriggerMode trigger_mode) {
  return trigger_mode == kAuto ? 0 : 1;
}

uint8 CMP_LOW(VoltRange range, double trig_volt, double offset_volt) {
  double div_volt = Volt(range)/kVoltDivNum;
  return static_cast<uint8>((trig_volt + offset_volt)/div_volt * 31.25 + 127);
}


double CalibrateInfo::Voffset(double offset) {
  return VCal_base + (offset) / Kcal;
}

uint32 CalibrateInfo::CH_OFFSET(double offset) {
  return static_cast<uint32>(
      (Voffset(offset) + 2.67) * pow(2.0, 16) / 4.2 / 1.25);
}

OscDevice::OscDevice(DeviceDelegate* device_delegate,
                     ConfigManager* config_manager)
    : DeviceBase(config_manager)
    , device_delegate_(device_delegate) {}


CalibrateInfo OscDevice::GetCalibrateInfo(Chnl chnl, VoltRange range) {
  // TODO get real Calibrate Info from hardware
  return kDefaultCalibrateInfo[range];
}

ChnlConfig OscDevice::GetChnlConfig(Chnl chnl) {
  ChnlConfig config;
  if (chnl == CAN_H) {
    config.range = range_can_h.value();
    config.offset = offset_can_h.value();
    config.coupling = coupling_can_h.value();
  } else if (chnl == CAN_L) {
    config.range = range_can_l.value();
    config.offset = offset_can_l.value();
    config.coupling = coupling_can_l.value();
  } else if (chnl == CAN_DIFF) {
    config.range = range_can_diff.value();
    config.offset = offset_can_diff.value();
    config.coupling = kDC; // no meaning
  }
  return config;
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
  soft_diff_.diff_ctrl.set_value(GetDiffCtrl(diff_ctrl.value()));
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
  TimeBase time_base_value = time_base.value();
  double time_offset_value = time_offset.value();
  trigger1_.div_coe.set_value(kCOE[time_base_value]);
  uint32 base_value = TimeBaseValue(time_base_value);
  uint32 samet = SAMET(trigger1_.div_coe.value());
  uint32 temp;
  uint32 k = DeviceK(device_type);
  temp = static_cast<uint32>(
      ((base_value*5*k + time_offset_value) /samet) * 2);
  temp &= 0xFFFFFFFC; // div by 4
  trigger1_.trig_pre.set_value(temp);
  temp = std::max(8U, static_cast<uint32>(
            ((base_value*5*k - time_offset_value)/samet) * 2));
  temp &= 0xFFFFFFFC; // div by 4
  trigger1_.trig_post.set_value(temp);
  trigger1_.auto_time.set_value(static_cast<uint32>(
      auto_time.value() * 1000000 / 10));
}

void OscDevice::SetTrigger2() {
  trigger2_.trig_source.set_value(trigger_source.value());
  trigger2_.trig_type.set_value(trigger_type.value());
  trigger2_.trig_mode.set_value(GetTrigMode(trigger_mode.value()));
  trigger2_.compare.set_value(compare.value());
  trigger2_.trig_time.set_value((static_cast<uint32>(time_param.value())/10 - 1)
      & 0x3FFFFFFF);

  if (!IsTriggerSourceChnl(trigger_source.value())) {
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
  ChnlConfig config = GetChnlConfig(TriggerSource2Chnl(trigger_source.value()));
  low = CMP_LOW(config.range, trigger_volt.value(), config.offset);

  if (trigger_sens.value() == kDefault) {
    high = low + 3;
  } else if (trigger_sens.value() == kStrengthen) {
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

device::Error OscDevice::WriteDeviceRange(::device::RegisterMemory& memory,
                                          int start_offset, int size) {
  DCHECK(start_offset + size <= memory.size());
  return device_delegate_->WriteDevice(
      memory.start_addr() + start_offset, memory.PtrByRelative(start_offset), size);
}

Error OscDevice::WriteDevice(RegisterMemory& memory) {
  return device_delegate_->WriteDevice(
    memory.start_addr(), memory.buffer(), memory.size());
}

Error OscDevice::ReadDevice(RegisterMemory& memory) {
  return device_delegate_->ReadDevice(
    memory.start_addr(), memory.buffer(), memory.size());
}

device::Error OscDevice::WriteSoftDiff() {
  return WriteDeviceRange(soft_diff_.memory, 
    SoftDiffRegister::kChnlOffset, SoftDiffRegister::kChnlSize);
}

#define CHECK_DEVICE(error) \
do { \
  if ((error) != canscope::device::OK) { \
    return; \
  } \
} while (0)

void OscDevice::SetVoltRange(Chnl chnl) {
  if (chnl != CAN_DIFF) {
    SetAnalogCtrl(chnl);
    SetAnalogSwitch(chnl);
  }
  SetSoftDiff(chnl);
  SetTrigger2();

  Error err;
  if (chnl != CAN_DIFF) {
    err = WriteDevice(analog_ctrl_.memory);
    CHECK_DEVICE(err);
    err = WriteDevice(analog_switch_.memory);
    CHECK_DEVICE(err);
  }
  err = WriteSoftDiff();
  CHECK_DEVICE(err);
  err = WriteDevice(trigger2_.memory);
}

void OscDevice::SetVoltOffset(Chnl chnl) {
  // same register as SetVoltOffset
  SetVoltRange(chnl);
}

void OscDevice::SetCoupling(Chnl chnl) {
  // CAN-DIFF no Coupling
  DCHECK(chnl != CAN_DIFF);
  SetAnalogSwitch(chnl);

  Error err;
  err = WriteDevice(analog_switch_.memory);
  CHECK_DEVICE(err);
}

void OscDevice::SetDiffCtrl() {
  // just set diff_ctrl
  SetSoftDiff(CAN_DIFF);
  Error err;
  err = WriteSoftDiff();
  CHECK_DEVICE(err);
}

void OscDevice::SetTimeBase() {
  SetTrigger1(device_type());
  Error err;
  err = WriteDevice(trigger1_.memory);
  CHECK_DEVICE(err);
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
  Error err;
  err = WriteDevice(trigger2_.memory);
  CHECK_DEVICE(err);
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


void OscDevice::SetAll() {
  SetAnalogCtrl(CAN_H);
  SetAnalogCtrl(CAN_L);
  SetSoftDiff(CAN_H);
  SetSoftDiff(CAN_L);
  SetSoftDiff(CAN_DIFF);
  SetAnalogSwitch(CAN_H);
  SetAnalogSwitch(CAN_L);
  SetTrigger1(device_type());
  SetTrigger2();

  Error err;
  err = WriteDevice(analog_ctrl_.memory);
  CHECK_DEVICE(err);
  err = WriteSoftDiff();
  CHECK_DEVICE(err);
  err = WriteDevice(analog_switch_.memory);
  CHECK_DEVICE(err);
  err = WriteDevice(trigger1_.memory);
  CHECK_DEVICE(err);
  err = WriteDevice(trigger2_.memory);
  CHECK_DEVICE(err);
}

} // namespace canscope
