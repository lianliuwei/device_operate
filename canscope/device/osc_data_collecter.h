#pragma once

#include "device/register_memory.h"

#include "canscope/device_errors.h"
#include "canscope/device/data_collecter.h"
#include "canscope/device/osc_raw_data.h"
#include "canscope/device/register/trigger_state_register.h"

namespace canscope {

class UsbPort;
class UsbPortDeviceDelegate;

// implement as state machine
class OscDataCollecter : public DataCollecter {
public:
  typedef base::Callback<void(OscRawDataHandle)> DataCollectedCallback;

  OscDataCollecter(DataCollectedCallback call_back, 
                   UsbPortDeviceDelegate* device_delegate, 
                   OscDevice* osc_device);
  
  enum State {
    STATE_LOAD_CALIBRATE,
    STATE_COLLECT,
    STATE_NONE,
  };

private:
  virtual ~OscDataCollecter() {}

  // implement DataCollecter
  virtual LoopState OnLoopRun() OVERRIDE;

  bool calibrated;
  

  device::Error DoLoop();
  void DoLoadCalibrate(LoopState* loop_state);
  void DoCollect(LoopState* loop_state);
  void SaveError(device::Error error);
  void DeviceOffine();

  // update property.
  device::Error UpdateTriggerState();
  // start scope
  device::Error StartScope();

  bool IsCollected();


  bool WriteDevice(::device::RegisterMemory* memory);
  bool ReadDevice(::device::RegisterMemory* memory);
  device::Error ReadData(OscRawDataHandle raw_data);

  State next_state_;

  OscRawDataDeviceConfigHandle LastConfig();
  OscRawDataDeviceConfigHandle last_config_;

  OscDevice* osc_device_;
  UsbPort* usb_port();
  UsbPortDeviceDelegate* device_delegate_;
  bool stop_by_offine_;
  device::Error rv_;

  TriggerStateRegister trigger_state_;
  DataCollectedCallback call_back_;
};

} // namespace canscope
