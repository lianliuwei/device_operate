#pragma once

// set to hardware
class DeviceDelegate {
public:
  // FPGA or arm control
  virtual bool WriteDevice(uint32 addr, uint8* buffer, int size) = 0;
  virtual bool ReadDevice(uint32 addr, uint8* buffer, int size) = 0;

  // I2C

protected:
  DeviceDelegate();
  virtual ~DeviceDelegate();
};