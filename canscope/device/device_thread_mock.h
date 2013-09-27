#pragma once

#include "base/lazy_instance.h"

#include "testing/gmock/include/gmock/gmock.h"

class DeviceThreadMock {
public:
  DeviceThreadMock() {}
  ~DeviceThreadMock() {}

  MOCK_METHOD0(IsDeviceThread, bool());

private:
  DISALLOW_COPY_AND_ASSIGN(DeviceThreadMock);
};

extern base::LazyInstance<DeviceThreadMock> g_DeviceThreadMock;
