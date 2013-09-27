#pragma once

#include "base/lazy_instance.h"

#include "testing/gmock/include/gmock/gmock.h"

class DeviceThreadMock {
public:
  MOCK_METHOD0(IsDeviceThread, bool());
};

extern base::LazyInstance<DeviceThreadMock> g_DeviceThreadMock;
