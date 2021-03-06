#include "canscope/device/frame_device/frame_data_collecter.h"

#include "canscope/device/register/frame_storage_register.h"

using namespace base;
using namespace canscope::device;

namespace canscope {

void FrameDataCollecter::DeviceOffine() {
  next_state_ = STATE_CLEAN_LEFTOVER;
  set_stop_by_offine(true);
}

void FrameDataCollecter::SaveError(device::Error error) {
  last_error_ = error;
}

DataCollecter::LoopState FrameDataCollecter::OnLoopRun() {
  CHECK_NE(STATE_NONE, next_state_);
  LoopState loop_state = STOP;
  {
    State state = next_state_;
    next_state_ = STATE_NONE;
    switch (state) {
      case STATE_CLEAN_LEFTOVER:
        DoCleanLeftover(&loop_state);
        break;

      case STATE_CHECK_COLLECT:
        DoCheckCollect(&loop_state);
        break;
      
      case STATE_WAIT_FULL:
        DoWaitFull(&loop_state);
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

device::Error FrameDataCollecter::ReadDevice(::device::RegisterMemory& memory) {
  return device_delegate_->ReadDevice(
      memory.start_addr(), memory.buffer(), memory.size());
}

#define CHECK_DEVICE(error) \
do { \
  if ((error) != canscope::device::OK) { \
    SaveError((error)); \
    DeviceOffine(); \
    return; \
  } \
} while (0)

void FrameDataCollecter::DoCleanLeftover(LoopState* loop_state) {
  *loop_state = STOP;
  if (!clean_leftover_) {
    next_state_ = STATE_CHECK_COLLECT;
    *loop_state = IMMEDIATE;
    return;
  } 

  device::Error err;
   // clean buffer
  while(1) {
    FrameStorageRegister frame_storage;
    err = ReadDevice(frame_storage.memory);
    CHECK_DEVICE(err);
    int frame_num =  frame_storage.frame_num.value();
    if (frame_num == 0) {
      break;
    }
    int read_size = frame_num > kFrameBufferMaxSize ? kFrameBufferMaxSize : frame_num;
    scoped_ptr<uint8[]> buffer(new uint8[read_size]);
    err = device_delegate_->ReadFrameData(buffer.get(), read_size);
    CHECK_DEVICE(err);
  }

  next_state_ = STATE_CHECK_COLLECT;
  *loop_state = IMMEDIATE;
}

void FrameDataCollecter::DoCheckCollect(LoopState* loop_state) {
  *loop_state = STOP;
  FrameStorageRegister frame_storage;
  device::Error err;
  err = ReadDevice(frame_storage.memory);
  CHECK_DEVICE(err);  
  if (frame_storage.overflow.value()) {
    LOG(WARNING) << "frame Overflow";
    // TODO notify overflow
  }
  int frame_num = frame_storage.frame_num.value();
  if (frame_num % kFrameSize != 0) {
    SaveError(ERR_FRAME_SIZE_NO_ALIGNED);
    return;
  }
  if (frame_num < kFrameBufferMaxSize) {
    next_loop_delay_ = TimeDelta::FromMilliseconds(kFrameWaitFull);
    next_state_ = STATE_WAIT_FULL;
    *loop_state = NEXT_LOOP;
  } else  {
    next_state_ = STATE_COLLECT;
    *loop_state = IMMEDIATE;
  }
}

void FrameDataCollecter::DoWaitFull(LoopState* loop_state) {
  next_state_ = STATE_COLLECT;
  *loop_state = IMMEDIATE;
}

void FrameDataCollecter::DoCollect(LoopState* loop_state) {
  *loop_state = STOP;
  FrameStorageRegister frame_storage;
  device::Error err;
  err = ReadDevice(frame_storage.memory);
  CHECK_DEVICE(err);  
  if (frame_storage.overflow.value()) {
    LOG(WARNING) << "frame Overflow";
    // TODO notify overflow
  }
  int frame_num = frame_storage.frame_num.value();
  if (frame_num % kFrameSize != 0) {
    SaveError(ERR_FRAME_SIZE_NO_ALIGNED);
    return;
  }
  // still no data
  if (frame_num == 0) {
    next_state_ = STATE_CHECK_COLLECT;
    // immediate do after PostTask, sleep long in WaitFull
    next_loop_delay_ = TimeDelta::FromMilliseconds(1);
    *loop_state = NEXT_LOOP;
    return;
  }
  int read_size = frame_num > kFrameBufferMaxSize ? kFrameBufferMaxSize : frame_num;
  FrameRawDataHandle raw_data = new FrameRawData(read_size);
  err = device_delegate_->ReadFrameData(raw_data->data(), raw_data->size());
  CHECK_DEVICE(err);
  // notify raw data
  raw_data->set_real_size(read_size);
  queue_->PushBulk(raw_data);

  next_state_ = STATE_CHECK_COLLECT;
  // immediate do after PostTask, sleep long in WaitFull
  next_loop_delay_ = TimeDelta::FromMilliseconds(1);
  *loop_state = !IsSingle() ? NEXT_LOOP : STOP;
}

FrameDataCollecter::FrameDataCollecter(DeviceDelegate* device_delegate, 
                                       FrameDevice* frame_device,
                                       bool clean_leftover)
    : device_delegate_(device_delegate)
    , frame_device_(frame_device)
    , next_state_(STATE_CLEAN_LEFTOVER)
    , last_error_(OK)
    , clean_leftover_(clean_leftover)
    , queue_(new FrameRawDataQueue(false, true)) {
  
}

#undef CHECK_DEVICE

} // namespace canscope
