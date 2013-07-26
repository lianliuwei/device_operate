#include "canscope/device/osc_device.h"

namespace {
// match to VoltRange
static const uint8 kSensCoeff[] = {4,8,16,32,80,200};

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


canscope::Chnl TriggerSource2Chnl(TriggerSource trigger_source) {
  switch (trigger_source) {
  case kTriggerSourceCANH: return CAN_H;
  case kTriggerSourceCANL: return CAN_L;
  case kTriggerSourceCANDIFF: return CAN_DIFF;
  default: NOTREACHED(); return CAN_H;
  }
}

void OscDevice::SetVoltBase(Chnl chnl) {
  ChnlConfig config = GetChnlConfig(chnl);
  CalibrateInfo info = GetCalibrateInfo(chnl, config.range);
  if (chnl == CAN_H) {
    analog_ctrl_.can_h_gain.set_value(info.Gain());
    soft_diff_.ch_sens_canh.set_value(kSensCoeff[config.range]);
    soft_diff_.ch_zero_canh.set_value(CH_ZERO(config.range, config.offset));
    analog_switch_.attenuation_canh.set_value(Attenuation(config.range));
  } else {
    soft_diff_.ch_sens_canl.set_value(kSensCoeff[config.range]);
    analog_ctrl_.can_l_gain.set_value(info.Gain());
    soft_diff_.ch_zero_canl.set_value(CH_ZERO(config.range, config.offset));
    analog_switch_.attenuation_canl.set_value(Attenuation(config.range));
  }
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
  trigger1_.SdivCoe.set_value(kCOE[time_base]);
  uint32 base_value = kTimeBaseValue[time_base];
  uint32 samet = SAMET(trigger1_.SdivCoe.value());
  uint32 temp;
  uint32 k = DeviceK(device_type);
  temp = static_cast<uint32>(((time_base*5*k + time_offset) /samet) * 2);
  temp &= 0xFFFFFFFC; // div by 4
  trigger1_.TrigPre.set_value(temp);
  temp = std::max(8U, 
      static_cast<uint32>(((time_base*5*k - time_offset)/samet) * 2));
  temp &= 0xFFFFFFFC; // div by 4
  trigger1_.TrigPost.set_value(temp);
  trigger1_.AutoTime.set_value(static_cast<uint32>(auto_time * 1000000 / 10));
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


} // namespace canscope
