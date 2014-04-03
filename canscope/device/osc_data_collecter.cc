#include "canscope/device/osc_data_collecter.h"

#include "canscope/device/canscope_device_constants.h"
#include "canscope/device/usb_port_device_delegate.h"
#include "canscope/device/register/scope_ctrl_register.h"
#include "canscope/device/register/canscope_device_register_constants.h"

using namespace base;
using namespace device;
using namespace canscope::device;

namespace canscope {

DataCollecter::LoopState OscDataCollecter::OnLoopRun() {
  CHECK_NE(STATE_NONE, next_state_);
  LoopState loop_state = STOP;
  {
    State state = next_state_;
    next_state_ = STATE_NONE;
    switch (state) {
      case STATE_LOAD_CALIBRATE:
        DoLoadCalibrate(&loop_state);
        break;
      
      case STATE_PRE_COLLECT:
        DoPreCollect(&loop_state);
        break;

      case STATE_CHECK_COLLECT:
        DoCheckCollect(&loop_state);
        break;
      
      case STATE_COLLECT:
        DoCollect(&loop_state);
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

  next_state_ = STATE_PRE_COLLECT;
  *loop_state = IMMEDIATE;
  return;
}

void OscDataCollecter::SaveError(Error error) {
  rv_ = error;
  // TODO may be save current stack
  LOG(INFO) << "OscDataCollecter stop by: " << ErrorToString(error);
}

void OscDataCollecter::DeviceOffine() {
  // TODO Check if need to load Calibrate 
  next_state_ = STATE_LOAD_CALIBRATE;
  set_stop_by_offine(true);
}

#define CHECK_DEVICE(error) \
do { \
  if ((error) != OK) { \
    SaveError((error)); \
    DeviceOffine(); \
    return; \
  } \
} while (0)

void OscDataCollecter::DoPreCollect(LoopState* loop_state) {
  *loop_state = STOP;

  Error error = OK;
  error = StartScope();
  CHECK_DEVICE(error);
  
  bool limit_time_ = GetLimitTime(&time_delta_);
  start_time_ = Time::Now();
  next_state_ = STATE_CHECK_COLLECT;
  *loop_state = IMMEDIATE;
}

void OscDataCollecter::DoCheckCollect(LoopState* loop_state) {
  *loop_state = STOP;

  Error error = OK;
  
  if (limit_time_) {
    if (Time::Now() - start_time_ > time_delta_) {
      CHECK_DEVICE(ERR_OSC_DEVICE_TIMEOUT_NOTTRIG);
    }
  }

  for (int i = 0; i < kOscCollectUpdateStateCount; ++i) {
    error = UpdateTriggerState();
    CHECK_DEVICE(error);
    if (IsCollected()) {
      next_state_ = STATE_COLLECT;
      *loop_state = IMMEDIATE;
      return;
    }
    if (i == kOscCollectUpdateStateCount - 1) {
      DeviceOffine();
      return;
    }
    SleepMs(kOscCollectUpdateStateInterval);
  }
  *loop_state = NEXT_LOOP;
}

void OscDataCollecter::DoCollect(LoopState* loop_state) {
  *loop_state = STOP;

  Error error = OK;

  OscRawDataHandle raw_data;
  bool ret = AllocOscRawData(&raw_data, osc_device_->device_type(), LastConfig());

  if (!ret) {
    LOG(WARNING) << "OscRawData pool full. may other part memory leak OscRawData";
    // TODO notify something
    *loop_state = NEXT_LOOP;
    return;
  }

  error = ReadData(raw_data);
  CHECK_DEVICE(error);
  
  queue_->PushBulk(raw_data);

  next_state_ = STATE_COLLECT;
  *loop_state = !IsSingle() ? NEXT_LOOP : STOP;
}

#undef CHECK_DEVICE

bool OscDataCollecter::WriteDevice(RegisterMemory* memory) {
  return device_delegate_->WriteDevice(
    memory->start_addr(), memory->buffer(), memory->size());
}

bool OscDataCollecter::ReadDevice(RegisterMemory* memory) {
  return device_delegate_->ReadDevice(
    memory->start_addr(), memory->buffer(), memory->size());
}

Error OscDataCollecter::ReadData(OscRawDataHandle raw_data) {
  memset(raw_data->data(), 0, raw_data->size());
  bool ret = usb_port()->ReadEP3(kScopeReadAddr, 
      kUsbModelStream, raw_data->data(), raw_data->size());
  return ret ? OK : ERR_READ_DEVICE;
}

Error OscDataCollecter::UpdateTriggerState() {
  bool ret = ReadDevice(&(trigger_state_.memory));
  return ret ? OK : ERR_READ_DEVICE;
}

Error OscDataCollecter::StartScope() {
  ScopeCtrlRegister scope_ctrl;
  scope_ctrl.scope_ctrl.set_value(true);
  bool ret = WriteDevice(&(scope_ctrl.memory));
  return ret ? OK : ERR_WRITE_DEVICE;
}

bool OscDataCollecter::IsCollected() {
  return trigger_state_.clet_bit.value();
}

OscRawDataDeviceConfigHandle OscDataCollecter::LastConfig() {
  ConfigManager::Config config = osc_device_->config_manager()->GetLast();
  if (last_config_.get() == NULL) {
    last_config_ = new OscRawDataDeviceConfig(config);
  } else if (!last_config_->SameConfig(config)) {
    last_config_ = new OscRawDataDeviceConfig(config);
  }
  return last_config_;
}

OscDataCollecter::OscDataCollecter(UsbPortDeviceDelegate* device_delegate, 
                                   OscDevice* osc_device)
    : device_delegate_(device_delegate)
    , osc_device_(osc_device)
    , next_state_(STATE_LOAD_CALIBRATE)
    , rv_(OK)
    , pool_(new MemoryUsagePool(kOscMemoryUsage))
    // over write no keep
    , queue_(new OscRawDataQueue(true, false)) {
  SetFreq(kOscMaxFreq);
}

UsbPort* OscDataCollecter::usb_port() {
  return device_delegate_->usb_port_ptr();
}

bool OscDataCollecter::AllocOscRawData(OscRawDataHandle* raw_data, 
                                       DeviceType type, 
                                       OscRawDataDeviceConfigHandle config) {
  scoped_refptr<PooledOscRawData> pool_raw_data 
      = new PooledOscRawData(type, config, pool_);
  while (!pool_raw_data->IsAllocOK()) {
    bool ret = queue_->RecycleOne();
    if (!ret) {
      return false;
    }
    pool_raw_data->AllocAgain();
  }
  *raw_data = pool_raw_data;
  return true;
}

bool OscDataCollecter::GetLimitTime(base::TimeDelta* time_delta) {
  if (osc_device_->trigger_mode.value() == kAuto) {
    *time_delta = TimeDelta::FromMicroseconds(osc_device_->time_param.value());
    // use mini 
    if ((*time_delta) < TimeDelta::FromMilliseconds(kOscCollectMiniTimeLimit)) {
      *time_delta = TimeDelta::FromMilliseconds(kOscCollectMiniTimeLimit);
    }
    return true;
  }
  return false;
}

} // namespace canscope
