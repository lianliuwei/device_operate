#pragma once

#include "base/basictypes.h"

#include "testing/gmock/include/gmock/gmock.h"

#include "canscope/device/property/device_property_store.h"

class DevicePropertyObserverMock 
    : public canscope::DevicePropertyStore::Observer {
public:
  DevicePropertyObserverMock() {}
  ~DevicePropertyObserverMock() {}

  MOCK_METHOD1(OnPreferenceChanged, void(const std::string&));

private:
  DISALLOW_COPY_AND_ASSIGN(DevicePropertyObserverMock);
};
