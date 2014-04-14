#include "canscope/device/test/scoped_open_device.h"

#include <tchar.h>

#include "base/file_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "canscope/device/register/device_info.h"
#include "base/path_service.h"

#define EXPECT_TRUE_OR_RET_FALSE(ret) \
  { \
    EXPECT_EQ(true, (ret)); \
    if(!(ret)) \
    return false; \
  }

using namespace base;

namespace canscope {

ScopeOpenDevice::ScopeOpenDevice(UsbPortDeviceDelegate* device_delegate) 
    : device_delegate_(device_delegate)
    , open_(false) {
  DCHECK(device_delegate);
  open_ = InitDevice(device_delegate);
}

ScopeOpenDevice::~ScopeOpenDevice() {
  if (open_)
    CloseDevice(device_delegate_->usb_port_ptr());
}

bool ScopeOpenDevice::InitDevice(UsbPortDeviceDelegate* device_delegate) {
  UsbPort* usb_port = device_delegate->usb_port_ptr();

  std::vector<string16> devices;
  bool ret = EnumDevices(&devices);
  EXPECT_TRUE_OR_RET_FALSE(ret && devices.size() > 0);

  ret = usb_port->OpenDevice(devices[0]);
  EXPECT_TRUE_OR_RET_FALSE(ret);

  DeviceInfo device_info;
  ret = device_delegate->GetDeviceInfo(&device_info);
  EXPECT_TRUE_OR_RET_FALSE(ret);
  // config FPGA
  if (device_info.fpage_version.value() == 0xFFFFFFFF) {
    return true;
  }

  std::string content;
  base::FilePath fpga_file;
  PathService::Get(base::DIR_EXE, &fpga_file);
  // pro use different FPGA file.
  fpga_file = fpga_file.Append(
    device_info.IsProVersion() ? L"CANScopePro.dll" : L"CANScope.dll");
  ret = file_util::ReadFileToString(fpga_file, &content);
  EXPECT_TRUE_OR_RET_FALSE(ret);
  ret = usb_port->DownloadFPGA((uint8*)(content.c_str()), content.size());
  EXPECT_TRUE_OR_RET_FALSE(ret);

  return true;
}

} // namespace canscope