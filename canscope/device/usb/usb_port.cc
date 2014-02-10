#include "canscope/device/usb/usb_port.h"

#include <SetupAPI.h>
#include <tchar.h>

#include "base/logging.h"
#include "base/stringprintf.h"
#include "base/threading/platform_thread.h"

// need kDeviceInfoSize
#include "canscope/device/register/canscope_device_register_constants.h"

using namespace base;
using namespace canscope;

namespace {
static const uint32 kTimeout = 10000;
static const uint32 kBufferSize = kUsbCommandSize + CMD_MAX_LEN;
static const int kMaxDevice = 20;

int CmdToPipeIndex(uint32 cmd) {
  switch (cmd) {
  case WREP1: return 0;
  case RDEP1: return 1;
  case WREP2: return 2;
  case RDEP2: return 3;
  case WREP3: return 4;
  case RDEP3: return 5;
  default: NOTREACHED(); return 0;
  }
}

int WriteHandleIndex(int port) {
   return port * 2 + 1;
}

int ReadHandleIndex(int port) {
  return port * 2;
}

bool UsbModel2Memory(UsbMode mode) {
  return kUsbModelStream == mode;
}

int To4Align(int size) {
  return size % 4 ? (size/4 + 1) *4 : size;
}

}

namespace canscope {  
bool EnumDevices(std::vector<string16>* devices) {
  DCHECK(devices);

  HDEVINFO hardwareDeviceInfo = SetupDiGetClassDevs(&GUID_CLASS_CANSCOPE, 
      NULL, NULL, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE);

  SP_DEVICE_INTERFACE_DATA infData;
  infData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
  for(int i = 0;  i< kMaxDevice; ++i) {
    if (!SetupDiEnumDeviceInterfaces(hardwareDeviceInfo, NULL, 
            &GUID_CLASS_CANSCOPE, i, &infData)) {
        return GetLastError() == ERROR_NO_MORE_ITEMS;
    }
    DWORD len;
    if(!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, 
            &infData, NULL,0, &len, NULL)) {
      DWORD er = GetLastError();
      if(er != ERROR_INSUFFICIENT_BUFFER)
        return false;
    }
    scoped_ptr<char[]> buffer(new char[len]);
    PSP_INTERFACE_DEVICE_DETAIL_DATA infDetail = 
        PSP_INTERFACE_DEVICE_DETAIL_DATA(buffer.get());
    infDetail->cbSize = sizeof(SP_INTERFACE_DEVICE_DETAIL_DATA);
    if(!SetupDiGetDeviceInterfaceDetail(hardwareDeviceInfo, 
            &infData, infDetail, len, &len, NULL)) {
      return false;
    }

    devices->push_back(string16(infDetail->DevicePath));
  }

  if (!SetupDiDestroyDeviceInfoList(hardwareDeviceInfo)) {
    return false;
  }
  return true;
}

UsbPort::UsbPort() {
  for (int i = 0; i < arraysize(pipes); ++i) {
    pipes[i] = INVALID_HANDLE_VALUE;
  }
}

UsbPort::~UsbPort() {
  CloseDevice();
}

bool UsbPort::GetDeviceInfo(uint8* buffer, int size) {
  DCHECK(size == kDeviceInfoSize);
  SetCmd(SSTQ, 0, kUsbModelNormal, NULL, 0);
  if (!SendCmd()) {
    return false;
  }
  memcpy(buffer, rsp_buffer.memory.buffer(), size);
  return true;
}

bool UsbPort::WritePort2Raw(uint8* buffer, int size) {
  
  int write_num = 0;
  while (true) {
    int need_write = (size - write_num) > DATA_MAX_LEN ?
        DATA_MAX_LEN : (size - write_num);
    int current_write;
    if (need_write%4) {
      // align to 4
      int len = To4Align(need_write);
      scoped_ptr<uint8[]> temp_buffer(new uint8[len]);
      memset(temp_buffer.get(), 0x00, len);
      memcpy(temp_buffer.get(), buffer + write_num, need_write);
      if (!WritePort(kPort2, &current_write, temp_buffer.get(), len)) {
        return false;
      }
      if (current_write != len) {
        return false;
      }
    } else {
      if (!WritePort(kPort2, &current_write, buffer + write_num, need_write)) {
        return false;
      }
      if (current_write != need_write) {
        return false;
      }
    }
    write_num += need_write;
    if (write_num == size)
      break;
    CHECK(write_num <= size);
  }
  
  PlatformThread::Sleep(TimeDelta::FromMilliseconds(100));

  int read_size = 0;
  rsp_buffer.reset();
  if (!ReadPort(kPort1, &read_size, rsp_buffer.memory.buffer(), rsp_buffer.size()) ||
      read_size <= 0) {
    return false;
  }
  LOG_IF (ERROR, rsp_buffer.cmd_id.value() == 0xFFFF) << "FPGA file is damaged";
  
  return rsp_buffer.cmd_id.value() == WRITE_SUCCESS;

}

bool UsbPort::DownloadFPGA(uint8* buffer, int size) {
  DCHECK(buffer);
  DCHECK(size > 0);
  // DownloadFPGA can no be 4 align, align here and patch 0 in WritePort2Raw
  SetCmd(DOWNLOAD_FPGA, 0, kUsbModelNormal, NULL, To4Align(size));
  if (!SendCmd())
    return false;
  
  return WritePort2Raw(buffer, size);
}

bool UsbPort::OpenDevice(string16 device_name) {
  DCHECK(!device_name.empty());
  for (int i = 0; i < arraysize(pipes); ++i) {
    string16 file_path = 
        StringPrintf(_T("%ls\\PIPE%02d"), device_name.c_str(), i);
    HANDLE temp = CreateFile(file_path.c_str(),
                      GENERIC_WRITE | GENERIC_READ,
                      0,
                      NULL,
                      OPEN_EXISTING,
                      FILE_FLAG_OVERLAPPED,
                      NULL);
    if (temp == INVALID_HANDLE_VALUE) {
      // TODO find out this state is, see CANScope BaseCommCmd.cpp
      return i >= 3;
    }
    pipes[i] = temp;
  }
  return true;
}

bool UsbPort::CloseDevice() {
  int fault_num = 0;
  for (int i = 0; i < arraysize(pipes); ++i) {
    if (pipes[i] != INVALID_HANDLE_VALUE) {
      if (CloseHandle(pipes[i])) {
        pipes[i] = INVALID_HANDLE_VALUE;
      } else {
        ++fault_num;
      }
    }
  }
  return fault_num == 0;
}

bool UsbPort::ReadPort(Port port, int* readed, uint8* buffer, int size) {
  HANDLE hPort = pipes[ReadHandleIndex(port)];
  if(hPort == INVALID_HANDLE_VALUE)
    return false;

  OVERLAPPED os;
  memset(&os, 0, sizeof(OVERLAPPED));
  os.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
  *readed = 0;
  BOOL bResult = TRUE;
  DWORD readed_unsigned;
  if (!ReadFile(hPort, buffer, size, &readed_unsigned, &os)) {
    DWORD dwErrCode = GetLastError();
    if (dwErrCode == ERROR_IO_PENDING) {	
      switch(WaitForSingleObject(os.hEvent, kTimeout)) {
      case WAIT_OBJECT_0:
        bResult = GetOverlappedResult(hPort, &os, &readed_unsigned, FALSE);
        break;

      case WAIT_TIMEOUT:
        ::CancelIo(hPort);
        bResult = FALSE;
        break;

      default:
        bResult = FALSE;
        break;
      };
    } else {
      bResult = FALSE;
    }
  }
  CloseHandle(os.hEvent);
  *readed = static_cast<int>(readed_unsigned);
  return !!bResult;
}

bool UsbPort::WritePort(Port port, int* written, uint8* buffer, int size) {
  HANDLE port_handle = pipes[WriteHandleIndex(port)];
  DCHECK(written);
  DCHECK(buffer);
  DCHECK(size > 0);

  if(port_handle == INVALID_HANDLE_VALUE)
    return false;

  OVERLAPPED os;
  memset(&os,0,sizeof(OVERLAPPED));
  os.hEvent=CreateEvent(NULL, TRUE, FALSE, NULL);
  *written = 0;
  BOOL result = TRUE;
  DWORD written_unsigned;
  if (!::WriteFile(port_handle, buffer, size, &written_unsigned, &os)) {
    DWORD dwErrCode = GetLastError();
    if (dwErrCode == ERROR_IO_PENDING) {	
      switch(WaitForSingleObject(os.hEvent, kTimeout)) {
      case WAIT_OBJECT_0:
        result = GetOverlappedResult(port_handle, &os, &written_unsigned, FALSE);
        break;

      case WAIT_TIMEOUT:
        ::CancelIo(port_handle);
        result = FALSE;
        break;

      default:
        ::CancelIo(port_handle);
        result=FALSE;
        break;
      };
    } else {
      // TODO add logging
      result = FALSE;
    }
  }
  CloseHandle(os.hEvent);
  *written = static_cast<int>(written_unsigned);
  return !!result;
}

void UsbPort::SetCmd(uint16 cmd, uint32 addr, UsbMode mode, 
                     uint8* buffer, int size) {
  DCHECK(size >= 0);
  // size may be other Port read size, no just the cmd size
  // DCHECK(size <= cmd_buffer.size());
  cmd_buffer.reset();
  cmd_buffer.cmd_id.set_value(cmd);
  cmd_buffer.mode.set_value(UsbModel2Memory(mode));
  cmd_buffer.addr.set_value(addr);
  // always set data size, used to get other port data
  cmd_buffer.data_size.set_value(static_cast<uint32>(size));
  if (buffer) {
    CHECK(size <= UsbCommand::write_data_max_size());
    memcpy(cmd_buffer.write_data_ptr(), buffer, static_cast<uint32>(size));
  }
}

bool UsbPort::SendCmd() {
  int size;
  if (!WritePort(kPort1, &size, cmd_buffer.memory.buffer(), cmd_buffer.size()) ||
      size != cmd_buffer.size()) {
    return false;
  }
  rsp_buffer.reset();
  // read response no limit return size to rsp_buffer size 
  // (hardware property, like GetDeviceInfo return 52 no 64)
  if (!ReadPort(kPort1, &size, rsp_buffer.memory.buffer(), rsp_buffer.size()) ||
      size <= 0) {
    return false;
  }
  return cmd_buffer.cmd_id.value() == rsp_buffer.cmd_id.value();
}

bool UsbPort::ReadEP1(uint32 addr, UsbMode mode, uint8* buffer, int size) {
  DCHECK(buffer);
  CHECK(size <= UsbCommand::read_data_max_size());
  SetCmd(RDEP1, addr, mode, NULL, size);
  if (!SendCmd()) {
    return false;
  }
  memcpy(buffer, rsp_buffer.read_data_ptr(), size);
  return true;
}

bool UsbPort::WriteEP1(uint32 addr, UsbMode mode, uint8* buffer, int size) {
  DCHECK(buffer);
  CHECK(size <= UsbCommand::write_data_max_size());
  SetCmd(WREP1, addr, mode, buffer, size);
  return SendCmd();
}

bool UsbPort::ReadEP2(uint32 addr, UsbMode mode, uint8* buffer, int size) {
  DCHECK(size%4 == 0); // TODO check read write EP2 need align to 4
  DCHECK(buffer);
  DCHECK(size > 0);
  SetCmd(RDEP2, addr, mode, NULL, size);
  if (!SendCmd())
    return false;

  int read_num = 0;
  while (true) {
    int need_read = (size - read_num) > DATA_MAX_LEN ? 
        DATA_MAX_LEN : (size - read_num);
    int current_read = 0;
    if (!ReadPort(kPort2, &current_read, buffer + read_num, need_read)) {
      return false;
    }
    if (current_read != need_read) {
      return false;
    }
    read_num += current_read;
    if (read_num == size)
      break;
    CHECK(read_num <= size);
  }
  return true;
}

bool UsbPort::WriteEP2(uint32 addr, UsbMode mode, uint8* buffer, int size) {
  DCHECK(size%4 == 0); // TODO check read write EP2 need align to 4
  DCHECK(buffer);
  DCHECK(size > 0);
  SetCmd(WREP2, addr, mode, NULL, size);
  if (!SendCmd())
    return false;

  return WritePort2Raw(buffer, size);
}

bool UsbPort::ReadEP3(uint32 addr, UsbMode mode, uint8* buffer, int size) {
  DCHECK(buffer);
  DCHECK(size > 0);
  SetCmd(RDEP3, addr, mode, NULL, size);
  if (!SendCmd())
    return false;

  int read_num = 0;
  while (true) {
    int need_read = (size - read_num) > DATA_MAX_LEN ? 
        DATA_MAX_LEN : (size - read_num);
    int current_read = 0;
    if (!ReadPort(kPort3, &current_read, buffer + read_num, need_read)) {
      return false;
    }
    if (current_read != need_read) {
      return false;
    }
    read_num += current_read;
    if (read_num == size)
      break;
    CHECK(read_num <= size);
  }
  return true;
}

} // namespace canscope
