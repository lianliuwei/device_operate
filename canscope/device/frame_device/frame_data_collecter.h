#pragma once

#include "device/sequenced_bulk_queue.h"
#include "device/register_memory.h"

#include "canscope/device_errors.h"
#include "canscope/device/data_collecter.h"
#include "canscope/device/frame_device/frame_raw_data.h"
#include "canscope/device/device_delegate.h"
#include "canscope/device/frame_device/frame_device.h"

namespace canscope {

typedef SequencedBulkQueue<FrameRawDataHandle> FrameRawDataQueue;

class FrameDataCollecter : public DataCollecter {
public:
  FrameDataCollecter(DeviceDelegate* device_delegate,
                     FrameDevice* frame_device,
                     bool clean_leftover);

  enum State {
    STATE_CLEAN_LEFTOVER,
    STATE_CHECK_COLLECT,
    STATE_WAIT_FULL,
    STATE_COLLECT,
    STATE_NONE,
  };

  scoped_refptr<FrameRawDataQueue> RawDataQueue() { return queue_; }

private:
  virtual ~FrameDataCollecter() {}

  // implement DataCollecter
  virtual LoopState OnLoopRun() OVERRIDE;

  void DoCheckCollect(LoopState* loop_state);
  void DoWaitFull(LoopState* loop_state);
  void DoCollect(LoopState* loop_state);
  void DoCleanLeftover(LoopState* loop_state);
  void SaveError(device::Error error);

  device::Error ReadDevice(::device::RegisterMemory& memory);
  void DeviceOffine();

  device::Error last_error_;
  State next_state_;
  FrameDevice* frame_device_;
  DeviceDelegate* device_delegate_;
  bool clean_leftover_;

  scoped_refptr<FrameRawDataQueue> queue_;

  DISALLOW_COPY_AND_ASSIGN(FrameDataCollecter);
};

} // namespace canscope
