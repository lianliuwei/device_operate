#include "canscope/device/test/scoped_open_device.h"

#include <tchar.h>

#include "base/file_util.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "canscope/device/register/device_info.h"
#include "base/path_service.h"

#define EXPECT_OK_OR_RET_FALSE(ret) \
{ \
  EXPECT_EQ(canscope::device::OK, (ret)); \
  if((ret) != canscope::device::OK) \
    return false; \
}

#define EXPECT_TRUE_OR_RET_FALSE(ret) \
{ \
  EXPECT_EQ(true, (ret)); \
  if(!(ret)) \
    return false; \
}

using namespace base;

namespace canscope {

ScopeOpenDevice::ScopeOpenDevice(DeviceDelegate* device_delegate) 
    : device_delegate_(device_delegate)
    , open_(false) {
  DCHECK(device_delegate);
  open_ = InitDevice(device_delegate, &type_);
}

ScopeOpenDevice::~ScopeOpenDevice() {
  if (open_)
    CloseDevice(device_delegate_);
}

bool ScopeOpenDevice::InitDevice(DeviceDelegate* device_delegate, DeviceType* type) {

  std::vector<string16> devices;
  device::Error err = device_delegate->EnumDevices(&devices);
  EXPECT_OK_OR_RET_FALSE(err);
  EXPECT_TRUE_OR_RET_FALSE(devices.size() > 0);

  err = device_delegate->OpenDevice(devices[0]);
  EXPECT_OK_OR_RET_FALSE(err);

  DeviceInfo device_info;
  err = device_delegate->GetDeviceInfo(&device_info);
  EXPECT_OK_OR_RET_FALSE(err);
  *type = device_info.GetDeviceType();
  // config FPGA
  if (device_info.fpage_version.value() != 0xFFFFFFFF) {
    return true;
  }

  std::string content;
  base::FilePath fpga_file;
  PathService::Get(base::DIR_EXE, &fpga_file);
  // pro use different FPGA file.
  fpga_file = fpga_file.Append(
    device_info.IsProVersion() ? L"CANScopePro.dll" : L"CANScope.dll");
  bool ret = file_util::ReadFileToString(fpga_file, &content);
  EXPECT_TRUE_OR_RET_FALSE(ret);
  err = device_delegate->DownloadFPGA((uint8*)(content.c_str()), content.size());
  EXPECT_OK_OR_RET_FALSE(err);

  return true;
}

} // namespace canscope