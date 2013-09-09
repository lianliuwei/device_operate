#include "canscope/device_errors.h"

#include "base/debug/trace_event.h"
#include "base/lazy_instance.h"
#include "base/threading/thread_local.h"

using namespace canscope::device;
namespace {
base::LazyInstance<base::ThreadLocalPointer<ScopedDeviceError> > 
    lazy_tls_ptr = LAZY_INSTANCE_INITIALIZER;
}

namespace canscope {
namespace device {

Error LastDeviceError() {
  return lazy_tls_ptr.Pointer()->Get()->last_error;
}

void SetDeviceError(Error error) {
  Error last_error = LastDeviceError();
  DCHECK(last_error == OK) << "must CleanError() before call device operate";
  lazy_tls_ptr.Pointer()->Get()->last_error = error;
  TRACE_EVENT_INSTANT0("device_error", "change", TRACE_EVENT_SCOPE_THREAD);
}


void SetDeviceErrorIfNoError(Error error) {
  Error last_error = LastDeviceError();
  if (last_error == OK) {
    lazy_tls_ptr.Pointer()->Get()->last_error = error;
    TRACE_EVENT_INSTANT0("device_error", "change", TRACE_EVENT_SCOPE_THREAD);
  }
}

void CleanError() {
  lazy_tls_ptr.Pointer()->Get()->last_error = OK;
}


ScopedDeviceError::ScopedDeviceError()
    : last_error(OK) {
  DCHECK(!lazy_tls_ptr.Pointer()->Get());
  lazy_tls_ptr.Pointer()->Set(this);
}

ScopedDeviceError::~ScopedDeviceError() {
  DCHECK_EQ(lazy_tls_ptr.Pointer()->Get(), this);
  lazy_tls_ptr.Pointer()->Set(NULL);
}

} // namespace device
} // namespace canscope