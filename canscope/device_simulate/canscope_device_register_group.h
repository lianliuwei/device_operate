#pragma once

#include "base/callback.h"

#include "canscope/device_errors.h"

#include "canscope/device/register/analog_ctrl_register.h"
#include "canscope/device/register/analog_switch_register.h"
#include "canscope/device/register/device_info.h"
#include "canscope/device/register/fpga_send_register.h"
#include "canscope/device/register/frame_storage_register.h"
#include "canscope/device/register/scope_ctrl_register.h"
#include "canscope/device/register/sja1000_register.h"
#include "canscope/device/register/soft_diff_register.h"
#include "canscope/device/register/trigger1_register.h"
#include "canscope/device/register/trigger_state_register.h"
#include "canscope/device/register/trigger2_register.h"
#include "canscope/device/register/wave_read_ctrl_register.h"
#include "canscope/device/register/wave_storage_register.h"

namespace canscope {

// read call before read, write call after write, if write fault roll back to before.
class CANScopeDeviceRegisterGroup {
public:
  CANScopeDeviceRegisterGroup();
  ~CANScopeDeviceRegisterGroup() {}

  AnalogCtrlRegister analog_ctrl;
  AnalogSwitchRegister analog_switch;
  DeviceInfo device_info;
  FpgaSendRegister fpga_send;
  FrameStorageRegister frame_storage;
  ScopeCtrlRegister scope_ctrl;
  SJA1000Register sja1000;
  SoftDiffRegister soft_diff;
  Trigger1Register trigger1;
  TriggerStateRegister trigger_state;
  Trigger2Register trigger2;
  WaveReadCtrlRegister wave_read_ctrl;
  WaveStorageRegister wave_storage;

  typedef base::Callback<device::Error(
      ::device::RegisterMemory* memory, uint32 offset, int size, bool read)> 
      RegisterCallback;

  device::Error WriteDevice(uint32 addr, uint8* buffer, int size);
  device::Error ReadDevice(uint32 addr, uint8* buffer, int size);
  
  void SetCallback(const ::device::RegisterMemory& memory, RegisterCallback callback);

private:
  struct RegisterRecord {
    ::device::RegisterMemory* memory;
    RegisterCallback callback;
  };

  void AddRegister(::device::RegisterMemory* memory);

  std::vector<RegisterRecord> registers_;

  DISALLOW_COPY_AND_ASSIGN(CANScopeDeviceRegisterGroup);
};

} // namespace canscope