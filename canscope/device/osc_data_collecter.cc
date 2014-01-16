#include "canscope/device/osc_data_collecter.h"

using namespace canscope::device;
namespace canscope {

LoopState OscDataCollecter::OnLoopRun() {
  CHECK_NE(STATE_NONE, next_state_);
  Error rv = OK;
  LoopState loop_state = STOP;
  {
    State state = next_state_;
    next_state_ = STATE_NONE;
    switch (state) {
      case STATE_LOAD_CALIBRATE:
        rv = DoLoadCalibrate(&loop_state);
        break;
      case STATE_COLLECT:
        rv = DoCollect(&loop_state);
        break;
      default:
        NOTREACHED();
        return STOP;
    }
  }
  // helper, if state machine just stop stop the loop.
  if (next_state_ == STATE_NONE)
    return STOP;
  return loop_state;
}

void OscDataCollecter::DoLoadCalibrate(LoopState* loop_state) {
  // TODO add load Calibrate
  calibrated = true;

  next_state_ = STATE_COLLECT;
  *loop_state = IMMEDIATE;
  return;
}

void OscDataCollecter::SaveError() {

}

void OscDataCollecter::DeviceOffine() {
  // TODO Check if need to load Calibrate 
  next_state_ = STATE_LOAD_CALIBRATE;
  set_stop_by_offine(true);
}

#define CHECK_DEVICE() \
do { \
  canscope::device::Error error = LastDeviceError(); \
  if (error != OK) { \
    SaveError(); \
    DeviceOffine(); \
    return; \
  } \
} while (0)

void OscDataCollecter::DoCollect(LoopState* loop_state) {
  *loop_state = STOP;

  osc_device_->Start();
  CHECK_DEVICE();
  for (int i = 0; i < kOscCollectUpdateStateCount; ++i) {
    osc_device_->UpdateTriggerState();
    CHECK_DEVICE();
    if (osc_device_->IsCollected())
      break;
    if (i == kOscCollectUpdateStateCount) {
      DeviceOffine();
      return;
    }
    SleepMilliseconds(kOscCollectUpdateStateInterval);
  }
  OscRawDataHandle raw_data(type_, LastConfig());
  memset(raw_data->data(), raw_data->size());
  osc_device_->ReadData(raw_data->data(), raw_data->size());
  CHECK_DEVICE();
  
  next_state_ = STATE_COLLECT;
  *loop_state = !IsSingle() ? NEXT_LOOP : STOP;

  return OK;
}

#undef CHECK_DEVICE


} // namespace canscope
