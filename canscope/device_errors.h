#pragma once

#include "base/basictypes.h"

namespace canscope {
namespace device {

enum Error {
  // No error.
  OK = 0,
#define DEVICE_ERROR(label, value) ERR_ ## label = value,
#include "canscope/device_error_list.h"
#undef DEVICE_ERROR
};

const char* ErrorToString(int error);

Error LastDeviceError();

// if last device error is no OK, this will fail
void SetDeviceError(Error error);

// if low level code set error use those more precise
void SetDeviceErrorIfNoError(Error error);

void CleanError();

class ScopedDeviceError {
public:
  ScopedDeviceError();
  ~ScopedDeviceError();

  Error last_error;

private:
  DISALLOW_COPY_AND_ASSIGN(ScopedDeviceError);
};


} // namespace device
} // namespace canscope
