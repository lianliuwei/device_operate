#include "wave_control/default_simple_ana_wave.h"


AnaWaveData& DefaultSimpleAnaWave::Data() {
  return *(data_.get());
}

WaveRange DefaultSimpleAnaWave::vertical_range() {
  return v_range_;
}

void DefaultSimpleAnaWave::set_vertical_range(const WaveRange& wave_range) {
  v_range_ = wave_range;
  NotifyChanged(kVertical);
}

WaveRange DefaultSimpleAnaWave::horizontal_range() {
  return h_range_;
}

void DefaultSimpleAnaWave::set_horizontal_range(const WaveRange& wave_range) {
  h_range_ = wave_range;
  NotifyChanged(kHorizontal);
}

void DefaultSimpleAnaWave::DoRangeCommand(int command_id, WaveRange range) {
  NOTREACHED();
}

void DefaultSimpleAnaWave::DoCommand(int command_id) {
  NOTREACHED();
}

DefaultSimpleAnaWave::DefaultSimpleAnaWave(string16 name, 
                                           SkColor color, 
                                           gfx::Image icon, 
                                           AnaWaveData* data)
    : name_(name)
    , color_(color)
    , icon_(icon)
    , data_(data) {
  SetRangeFromData();
}

void DefaultSimpleAnaWave::SetData(AnaWaveData* data) {
  data_.reset(data);
  NotifyChanged(kData);
}

void DefaultSimpleAnaWave::SetRangeFromData() {
  {
  WaveRange range = data_->data_range();
  double size = range.end - range.begin;
  range.end += size * 0.05;
  range.begin -= size * 0.05;
  h_range_ = range;
  }
  {
  WaveRange range;
  range.begin = data_->MaxY();
  range.end = data_->MinY();
  double size = range.begin - range.end;
  range.begin += size * 0.15;
  range.end -= size * 0.15;
  v_range_ = range;
  }

}
