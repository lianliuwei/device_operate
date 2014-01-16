#pragma once

namespace canscope {

// init the device first and download the FPGA.
// carefully with loop refcounted!!!!
// live in CANScopeDeviceManager
class CANScopeRunner {
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

  bool auto_init_device;
  bool start_on_device_online;
  bool rerun_on_back_online;

private:
  device::Error InitDeviceImpl(string16 device_path);
  void CloseDeviceImpl();

  string16 device_path_;
  CANScopeDeviceManager* canscope_;
  DeviceInfo device_info_;
  bool inited_;
};

} // namespace canscope
