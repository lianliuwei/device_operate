#include "canscope/test/test_util.h"

#include "base/threading/platform_thread.h"
#include "base/time.h"
#include "base/stringprintf.h"
#include "base/json/json_string_value_serializer.h"
#include "base/json/json_reader.h"
#include "testing/gtest/include/gtest/gtest.h"

#include "canscope/test/speed_meter.h"

using namespace std;
using namespace base;

void SleepMS(int64 ms) {
  PlatformThread::Sleep(TimeDelta::FromMilliseconds(ms));
}

void SleepDelta(base::TimeDelta delta) {
  PlatformThread::Sleep(delta);
}

bool SpeedSense::DeltaPass() {
  TimeDelta pass_time = Time::Now() - last_;
  if (pass_time < delta_) {
    return false;
  }
  return true;
}

DictionaryValue* GetConfig(const char* config) {
  std::string content(config);
  JSONStringValueSerializer serializer(content);
  int error;
  std::string error_msg;
  Value* value = serializer.Deserialize(&error, &error_msg);
  EXPECT_TRUE(NULL != value);
  EXPECT_TRUE(value->IsType(Value::TYPE_DICTIONARY));
  DictionaryValue* dict_value;
  value->GetAsDictionary(&dict_value);

  return dict_value;
}

base::TimeDelta SpeedSense::NextDelta() {
  TimeDelta pass_time = Time::Now() - last_;
  if (pass_time < delta_) {
    return delta_ - pass_time;
  } else {
    return TimeDelta();
  }
}

void SpeedSense::Update(int64 size, int64 count) {
  TimeDelta pass_time = Time::Now() - last_;

  last_size_ = total_size_;
  last_count_ = total_count_;

  total_size_ = size;
  total_count_ = count;

  size_speed_ = (total_size_ - last_size_) / pass_time.InSecondsF();
  count_speed_ = (total_count_ - last_count_) / pass_time.InSecondsF();

  last_ = Time::Now();
}

void SpeedSense::Start() {
  last_ = Time::Now();
  last_size_ = 0;
  last_count_ = 0;
  total_size_ = 0;
  total_count_ = 0;
  size_speed_ = 0;
  count_speed_ = 0;
}

std::string SpeedSense::FormatSpeed() {
  return StringPrintf("speed:%s. count_speed:%d", 
        FormatByte(size_speed_).c_str(), 
        count_speed_);
}

std::string SpeedSense::FormatSpeedAndTotal() {
  return StringPrintf("speed:%s. count_speed:%d. t_size:%s t_count:%lld.", 
        FormatByte(size_speed_).c_str(), 
        count_speed_,
        FormatByte(total_size_).c_str(),
        total_count_);
}

SpeedSense::SpeedSense(base::TimeDelta delta)
    : delta_(delta)
    , last_(Time::Now()) {
  Start();      
}

SpeedSense::SpeedSense(int64 ms)
    : delta_(TimeDelta::FromMilliseconds(ms))
    , last_(Time::Now()) {
  Start();
}
