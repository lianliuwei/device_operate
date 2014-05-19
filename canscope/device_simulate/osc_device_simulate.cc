#include "canscope/device_simulate/osc_device_simulate.h"

#define _USE_MATH_DEFINES
#include <math.h>
#undef  _USE_MATH_DEFINES

#include "base/time.h"

#include "canscope/device/config_util.h"

using namespace base;
using namespace device;
using namespace canscope::device;
using namespace canscope;

namespace {

static const double kVRange = 3; // unit V
static const double kHMoveUnit = 1900; // ns  each s wave move
static const double kHSinSize = 500; // ns
static const double LLHSize = 4000;

uint8 ToChnlValue(double v_range, double value) {
  double temp = (value + v_range/2) * 250 / v_range;
  if (temp < 0) {
    temp = 0;
  } else if (temp > 250) {
    temp = 255;
  } else {
    temp += 3;
  }
  return static_cast<uint8>(temp);
}

double LowHeight(double i) {
  double v = sin(i * M_PI / 2);
  return v > 0 ? 1.0 : -1.0;
}

uint32 SAMET(uint32 coe) {
  return (coe + 1) * 10;
}

TimeBase COEToTimeBase(uint32 time_base) {
  switch (time_base) {
  case 0: return k1us;
  case 1: return k2us;
  case 4: return k5us;
  case 9: return k10us;
  case 19: return k20us;
  case 49: return k50us;
  case 99: return k100us;
  case 199: return k200us;
  case 499: return k500us;
  case 999: return k1ms;
  case 1999: return k2ms;
  case 4999: return k5ms;
  case 9999: return k10ms;
  case 19999: return k20ms;
  case 49999: return k50ms;
  case 99999: return k100ms;
  case 199999: return k200ms;
  case 499999: return k500ms;
  case 1999999: return k1s;
  default:
    NOTREACHED();
    return k1us;
  }
}

VoltRange COEToVoltRange(uint8 coe) {
  switch (coe) {
  case 4: return k1V;
  case 8: return k2V;
  case 16: return k4V;
  case 32: return k8V;
  case 80: return k20V;
  case 200: return k50V;
  default: 
    NOTREACHED();
    return k1V;
  }
}

double ChZeroToOffset(VoltRange range, uint16 ch_zero) {
  return (ch_zero - 127) * Volt(range) / 250;
}

}

namespace canscope {

Error OscDeviceSimulate::OnTriggerState(RegisterMemory* memory, 
                                        uint32 offset, int size, bool read) {
  if (!read) {
    return OK;
  }
  group_->trigger_state.pre_bit.set_value(true);
  group_->trigger_state.trig_bit.set_value(true);
  group_->trigger_state.clet_bit.set_value(true);
  return OK;
}

device::Error OscDeviceSimulate::ReadOscData(uint8* buffer, int size) {
  DCHECK(buffer);
  DCHECK(size > 0);
  DCHECK(size %2 == 0);
  UpdateVoltConfig();
  UpdateTimeBase();
  UpdateTriggerBase();

  int chnl_size = size / 2;
  scoped_ptr<double[]> can_h(new double[chnl_size]);
  scoped_ptr<double[]> can_l(new double[chnl_size]);
  double wave_move = (Time::Now() - start_time_).InSeconds() * kHMoveUnit;
  double left_offset = time_offset.value();
  double screen_range = TimeBaseValue(time_base.value());
  int produce_size = 2000;
  for (int i = 0; i < produce_size; ++i) {
    {
    double v_offset = offset_can_h.value();
    double y_value = screen_range / chnl_size * i - wave_move + left_offset;
    can_h[i] = (kVRange/2) * sin(y_value / kHSinSize * M_PI / 2) + v_offset;
    }
    {
    double v_offset = offset_can_l.value();
    double y_value = screen_range / chnl_size * i - wave_move + left_offset;
    can_l[i] = (kVRange/2) * LowHeight(y_value / kHMoveUnit) + v_offset;
    }
  }
  double l_v_range = Volt(range_can_l.value());
  double h_v_range = Volt(range_can_h.value());
  for (int i = 0; i < produce_size; ++i) {
    buffer[i*2] = ToChnlValue(l_v_range, can_l[i]);
    buffer[i*2+1] = ToChnlValue(h_v_range, can_h[i]);
  }
  return OK;
}

void OscDeviceSimulate::UpdateVoltConfig() {
  uint8 coe = group_->soft_diff.ch_sens_canh.value();
  range_can_h.set_value(COEToVoltRange(coe));
  uint16 zero = group_->soft_diff.ch_zero_canh.value();
  offset_can_h.set_value(ChZeroToOffset(range_can_h.value(), zero));

  coe = group_->soft_diff.ch_sens_canl.value();
  range_can_l.set_value(COEToVoltRange(coe));
  zero = group_->soft_diff.ch_zero_canl.value();
  offset_can_l.set_value(ChZeroToOffset(range_can_l.value(), zero));
}

void OscDeviceSimulate::UpdateTimeBase() {
  uint32 coe = group_->trigger1.div_coe.value();
  time_base.set_value(COEToTimeBase(coe));
}

void OscDeviceSimulate::UpdateTriggerBase() {
  TimeBase time_base_value = time_base.value();
  uint32 base_value = TimeBaseValue(time_base_value);
  uint32 samet = SAMET(group_->trigger1.div_coe.value());
  uint32 k = DeviceK(device_type_);
  uint32 pre_value = group_->trigger1.trig_pre.value();
  double time_offset_value = pre_value*samet/2 - base_value*5*k;
  time_offset.set_value(time_offset_value);
}

OscDeviceSimulate::OscDeviceSimulate(CANScopeDeviceRegisterGroup* group, 
                                     DeviceType device_type)
    : group_(group)
    , device_type_(device_type) {
  group->SetCallback(group->trigger_state.memory, 
      Bind(&OscDeviceSimulate::OnTriggerState, Unretained(this)));
  Init(GetDefaultOscDeviceConfig());
}

} // namespace canscope