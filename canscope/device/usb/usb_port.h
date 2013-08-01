#pragma once

#include <Windows.h>
#include <vector>

#include "base/string16.h"
#include "base/memory/scoped_ptr.h"

#include "canscope/device/usb/usb_constants.h"
#include "canscope/device/usb/usb_command.h"

namespace canscope {

struct UsbCmd {

};

bool EnumDevices(std::vector<string16>* devices);

class UsbPort {
public:
  UsbPort();
  ~UsbPort();
  
  bool OpenDevice(string16 device_name);
  bool CloseDevice();

  // command
  bool GetDeviceInfo(uint8* buffer, int size);

  // FPGA can only download once when device power up.
  bool DownloadFPGA(uint8* buffer, int size);

  bool ReadEP1(uint32 addr, UsbMode mode, uint8* buffer, int size);

  bool WriteEP1(uint32 addr, UsbMode mode, uint8* buffer, int size);

  bool ReadEP2(uint32 addr, UsbMode mode, uint8* buffer, int size);

  bool WriteEP2(uint32 addr, UsbMode mode, uint8* buffer, int size);

  bool ReadEP3(uint32 addr, UsbMode mode, uint8* buffer, int size);

  bool ReadI2C(uint8 addr, uint8* eaddr, int eaddr_size, 
      uint32* buffer, int size);

  bool WriteI2C(uint8 addr, uint32* buffer, int size);

private:
  enum Port {
    kPort1,
    kPort2,
    kPort3,
  };
  // setCmd to cmd_buffer
  void SetCmd(uint16 cmd, uint32 addr, UsbMode mode, uint8* buffer, int size);
  // send cmd_buffer, and if success, read respond to rsp_buffer.
  bool SendCmd();

  // raw operate
  bool ReadPort(Port port, int* readed, uint8* buffer, int size);
  bool WritePort(Port port, int* written, uint8* buffer, int size);
  bool WritePort2Raw(uint8* buffer, int size);

  UsbCommand cmd_buffer;
  UsbCommand rsp_buffer;

  // device port handle
  // 3 EndPoint * 2 read write channel.
  HANDLE pipes[6];
};

} // namespace canscope
