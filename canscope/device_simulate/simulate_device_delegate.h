#pragma once

#include "base/threading/thread_checker.h"

#include "canscope/device/device_delegate.h"
#include "canscope/device_simulate/canscope_device_register_group.h"

namespace canscope {

class SimulateDeviceDelegate : public DeviceDelegate {
public:
  SimulateDeviceDelegate(bool check_thread);
  virtual ~SimulateDeviceDelegate() {}

  // implement DeviceDelegate
  virtual device::Error EnumDevices(std::vector<string16>* devices);
  virtual device::Error OpenDevice(string16 device_path);
  virtual device::Error CloseDevice();
  virtual device::Error DownloadFPGA(uint8* buffer, int size);
  virtual device::Error WriteDevice(uint32 addr, uint8* buffer, int size);
  virtual device::Error ReadDevice(uint32 addr, uint8* buffer, int size);
  virtual device::Error GetDeviceInfo(DeviceInfo* device_info);
  virtual device::Error ReadOscData(uint8* buffer, int size);
  virtual device::Error ReadFrameData(uint8* buffer, int size);
  virtual void DetachFromThread() { thread_check_.DetachFromThread(); }

protected:
  CANScopeDeviceRegisterGroup group_;

  void CheckOpen();
  void CheckConfig();
  void CheckThread();


private:
  base::ThreadChecker thread_check_;

  bool opened_;
  bool load_fpga_;
  bool check_thread_;
  
};

} // namespace canscope