#pragma once

#include "base/time.h"

#include "device/sequenced_bulk_queue.h"
#include "device/memory_usage_pool.h"

#include "canscope/device_errors.h"
#include "canscope/device/data_collecter.h"
#include "canscope/device/osc_raw_data.h"
#include "canscope/device/register/trigger_state_register.h"

namespace canscope {

class UsbPort;
class UsbPortDeviceDelegate;

typedef SequencedBulkQueue<OscRawDataHandle> OscRawDataQueue;

// implement as state machine
class OscDataCollecter : public DataCollecter {
public:
  OscDataCollecter(UsbPortDeviceDelegate* device_delegate, 
                   OscDevice* osc_device);
  
  enum State {
    STATE_LOAD_CALIBRATE,
    STATE_PRE_COLLECT,
    STATE_CHECK_COLLECT,
    STATE_COLLECT,
    STATE_NONE,
  };
  // 
  scoped_refptr<OscRawDataQueue> RawDataQueue() { return queue_; }

private:
  virtual ~OscDataCollecter() {}

  // implement DataCollecter
  virtual LoopState OnLoopRun() OVERRIDE;

  bool calibrated;
  
  void DoLoadCalibrate(LoopState* loop_state);
  void DoPreCollect(LoopState* loop_state);
  void DoCheckCollect(LoopState* loop_state);
  void DoCollect(LoopState* loop_state);

  void SaveError(device::Error error);
  void DeviceOffine();

  bool GetLimitTime(base::TimeDelta* time_delta);

  // update property.
  device::Error UpdateTriggerState();
  // start scope
  device::Error StartScope();

  bool IsCollected();

  bool AllocOscRawData(OscRawDataHandle* raw_data, 
                       DeviceType type, 
                       OscRawDataDeviceConfigHandle config);

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

  scoped_refptr<MemoryUsagePool> pool_;
  scoped_refptr<OscRawDataQueue> queue_;

  bool limit_time_;
  base::TimeDelta time_delta_;
  base::Time start_time_;

  DISALLOW_COPY_AND_ASSIGN(OscDataCollecter);
};

} // namespace canscope
