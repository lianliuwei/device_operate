#include "canscope/osc_chnl_calc/freq_time.h"

#include "base/logging.h"

FreqTime::FreqTime(double freq) {
  set_freq(freq);
}

bool FreqTime::Hit() {
  bool notify = false;
  if (last_notify_time_.is_null()) {
    notify = true;
    last_notify_time_ = base::Time::Now();
  } else {
    int64 micro_second = static_cast<int64>(1.0 / freq_ * 1e6);
    base::TimeDelta delta = base::TimeDelta::FromMicroseconds(micro_second);
    if (base::Time::Now() >= last_notify_time_ + delta) {
      notify = true;
      // no now + delta, keep the freq.
      last_notify_time_ = last_notify_time_ +  delta;
    }
  }
  return notify;
}

void FreqTime::set_freq(double freq) {
  DCHECK(freq > 0);
  freq_ = freq;
}

