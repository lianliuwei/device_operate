#pragma once

#include "base/basictypes.h"
#include "base/values.h"

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

// caller take own of DictonaryValue
base::DictionaryValue* ErrorAsDictionary(canscope::device::Error error);
canscope::device::Error ErrorFromDictonary(base::Value* value);

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
