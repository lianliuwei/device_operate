#pragma once

#include <string>

#include "base/time.h"

class SpeedMeter {
public:
  explicit SpeedMeter(base::TimeDelta delta);
  explicit SpeedMeter(int64 ms);
  ~SpeedMeter() {}

  void Start();

  // delta pass, need set data 
  bool DeltaPass();

  void set_size(int64 size);

  std::string FormatSpeed();
  std::string FormatSpeedAndTotal();

  int64 size_speed() const { return size_speed_; }
  int count_speed() const { return count_speed_; }

  int64 total_count() const { return total_count_; }
  int64 total_size() const { return total_size_; }

private:
  base::TimeDelta delta_;
  base::Time last_;

  int64 size_speed_;
  int count_speed_;

  int64 last_size_;
  int64 last_count_;
  int64 total_size_;
  int64 total_count_;

  DISALLOW_COPY_AND_ASSIGN(SpeedMeter);
};
  
std::string FormatByte(int64 byte_size);