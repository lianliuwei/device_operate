#include "canscope/device/frame_device/frame_device_handle.h"

#include "canscope/device/sync_call.h"

using namespace base;

namespace {
static const int kFpgaSendStateFreq = 40;
}

namespace canscope {

device::Error canscope::FrameDeviceHandle::FpgaSend(const FpgaFrameData& data,
                                                    uint8 send_num) {
  fpga_send.set_frame_data(data);
  fpga_send.send_num.set_value(send_num);
  // TODO need calc from data
  fpga_send.bit_num.set_value(20*8);
  fpga_send.btr_div.set_value(properties_.BtrDiv());
  fpga_send.start_send.set_value(true);

  return FpgaSend();
}

device::Error FrameDeviceHandle::FpgaSend() {
  SyncCall sync_call(DeviceTaskRunner());
  last_err_ = device::OK;
  sync_call.CallClosure(Bind(&FrameDeviceHandle::FpgaSendImpl, Unretained(this)));
  return last_err_;
}

// sync can use post fpga_send_ because it's in wait.
void FrameDeviceHandle::FpgaSendImpl() {
  device::Error err;
  err = device_->WriteDeviceRange(fpga_send.memory,
      fpga_send.SendOffset(), fpga_send.SendSize());
  if (err != device::OK) {
    last_err_ = err;
    return;
  }

  err = device_->WriteDeviceRange(fpga_send.memory,
      fpga_send.StateOffset(), fpga_send.StateSize());
  if (err != device::OK) {
    last_err_ = err;
    return;
  }
  

  int count = 0;
  while (true) {
    err = device_->ReadDeviceRange(fpga_send.memory,
        fpga_send.StateOffset(), fpga_send.StateSize());
    if (err != device::OK) {
      last_err_ = err;
      return;
    }
    // success
    if (fpga_send.send_state.value()) {
      last_err_ = device::OK;
      return;
    }
    // sleep
    if ((count % kFpgaSendStateFreq) == kFpgaSendStateFreq -1) {
      SleepMs(1);
    }
    // time out
    if (count > fpga_send.send_num.value() * 2 * kFpgaSendStateFreq)  {
      last_err_ = device::ERR_FPGA_SEND_TIMEOUT;
      return;
    }
    ++count;
  }
}

FrameDeviceHandle::FrameDeviceHandle(FrameDevice* device)
    : DeviceHandleBase(device)
    , device_(device)
    , last_err_(device::OK) {
  InitHandle();
  properties_.Init();
}

} // namespace canscope
