#pragma once

#include "base/time.h"

class FreqTime {
public:
  FreqTime(double freq);
  ~FreqTime() {}

  // freq > 0
  double freq() const { return freq_; }
  void set_freq(double freq);

  bool Hit();

private:
  // unit is Hz.
  double freq_;
  base::Time last_notify_time_;
};
