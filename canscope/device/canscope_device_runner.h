#pragma once

#include "canscope/device/osc_data_collecter.h"
#include "canscope/device/register/device_info.h"
#include "canscope/device/devices_manager_observer.h"
#include "canscope/device/canscope_device_finder.h"

namespace canscope {

class CANScopeDeviceManager;

// init the device first and download the FPGA.
// carefully with loop refcounted!!!!
// live in CANScopeDeviceManager
 class CANScopeRunner : public DevicesManagerObserver {
public:
  scoped_refptr<OscDataCollecter> osc_data;

  void StartAll();
  void StopAll();
  void ReRunAll();

  CANScopeRunner(CANScopeDeviceManager* canscope);
  virtual ~CANScopeRunner();

  void Init(string16 device_path);

  device::Error InitDevice();
  bool IsInited() const;
  device::Error ReOnline();
  void CloseDevice();

  bool auto_init_device;
  bool start_on_device_online;
  bool rerun_on_back_online;

private:
  device::Error InitDeviceImpl(string16 device_path);
  void CloseDeviceImpl();

  // 
  virtual void DeviceStateChanged();
  virtual void DeviceListChanged();

  string16 device_path_;
  CANScopeDeviceManager* canscope_;
  DeviceInfo device_info_;
  bool inited_;
  DeviceStatus status_;
};

} // namespace canscope
