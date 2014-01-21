#pragma once

#include "canscope/device_errors.h"
#include "canscope/device/data_collecter.h"
#include "canscope/device/osc_raw_data.h"

namespace canscope {

// implement as state machine
class OscDataCollecter : public DataCollecter {
public:
  typedef base::Callback<void(OscRawDataHandle)> DataCollectedCallback;

  OscDataCollecter(DataCollectedCallback call_back);
  
  enum State {
    STATE_LOAD_CALIBRATE,
    STATE_COLLECT,
    STATE_NONE,
  };

private:
  // implement DataCollecter
  virtual LoopState OnLoopRun() OVERRIDE;

  bool calibrated;
  

  device::Error DoLoop();
  bool DoLoadCalibrate();
  bool DoCollect();

  void OnDataCollected();

  OscDevice* osc_device_;
  State next_state_;

  DeviceType type_;
  OscRawDataDeviceConfig LastConfig();
  UsbPort* usb_port_;
  bool stop_by_offine_;
  device::Error rv_;
};

} // namespace canscope
