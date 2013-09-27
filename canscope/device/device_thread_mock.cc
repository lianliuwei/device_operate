#include "canscope/device/device_thread_mock.h"

base::LazyInstance<DeviceThreadMock> g_DeviceThreadMock;

namespace canscope {

 bool IsDeviceThread() {
  return g_DeviceThreadMock.Pointer()->IsDeviceThread();
}

}
