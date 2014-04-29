#pragma once

#include <string>

#include "base/time.h"
#include "base/basictypes.h"
#include "base/values.h"

void SleepMS(int64 ms);
void SleepDelta(base::TimeDelta delta);

// parse config, fault will EXPECT_TRUE
base::DictionaryValue* GetConfig(const char* config);

class SpeedSense {
public:
  explicit SpeedSense(base::TimeDelta delta);
  explicit SpeedSense(int64 ms);
  ~SpeedSense() {}

  void Start();

  bool DeltaPass();

  void Update(int64 size, int64 count);

  base::TimeDelta NextDelta();

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
};