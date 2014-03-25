#include "canscope/test/speed_meter.h"

#include "base/stringprintf.h"

using namespace std;
using namespace base;

namespace {
string FormatByte(int64 byte_size) {
  int64 temp = byte_size;
  int i = 0;
  while (temp / 1024 > 0) {
    ++i;
    temp = temp /1024;
  }
  static const char* ByteSuffix[] = { "B", "KB", "MB", "GB", "TB" };
  if (i > arraysize(ByteSuffix)) {
    i = arraysize(ByteSuffix);
  } 
  return StringPrintf("%.2lf %s", byte_size / pow(1024.0, i), ByteSuffix[i]);
 
}

}

void SpeedMeter::Start() {
  last_ = Time::Now();
  last_size_ = 0;
  last_count_ = 0;
  total_size_ = 0;
  total_count_ = 0;
  size_speed_ = 0;
  count_speed_ = 0;
}

void SpeedMeter::set_size(int64 size) {
  total_size_ += size;
  ++total_count_;
}

bool SpeedMeter::DeltaPass() {
  TimeDelta pass_time = Time::Now() - last_;
  if (pass_time < delta_) {
    return false;
  }

  size_speed_ = (total_size_ - last_size_) / pass_time.InSecondsF();
  count_speed_ = (total_count_ - last_count_) / pass_time.InSecondsF();

  last_ = Time::Now();
  last_size_ = total_size_;
  last_count_ = total_count_;

  return true;
}


SpeedMeter::SpeedMeter(base::TimeDelta delta)
    : delta_(delta) {
  Start();
}

std::string SpeedMeter::FormatSpeed() {
  return StringPrintf("speed:%s. count_speed:%d", 
        FormatByte(size_speed_).c_str(), 
        count_speed_);
}

std::string SpeedMeter::FormatSpeedAndTotal() {
  return StringPrintf("speed:%s. count_speed:%d. t_size:%s t_count:%lld.", 
        FormatByte(size_speed_).c_str(), 
        count_speed_,
        FormatByte(total_size_).c_str(),
        total_count_);
}
