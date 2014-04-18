#include "wave_control/examples/test_osc_wave.h"

#define _USE_MATH_DEFINES
#include <math.h>

AnaWaveData& TestOscWave::Data() {
  return *(wave_data_.get());
}

void TestOscWave::MoveToX(double pos) {
  horiz_->offset = pos;
  horiz_->NotifyChanged(kHorizontalOffset);
}

void TestOscWave::MoveToY(double pos) {
  vertical_->offset = pos;
  NotifyChanged(kVerticalOffset);
}

void TestOscWave::MoveTrigger(double pos) {
  trigger_->offset = pos;
  trigger_->NotifyChanged(kTriggerOffset);
}

void TestOscWave::ZoomInX() {
  NOTREACHED();
}

void TestOscWave::ZoomOutX() {
  NOTREACHED();
}

void TestOscWave::ZoomInY() {
  NOTREACHED();
}

void TestOscWave::ZoomOutY() {
  NOTREACHED();
}

bool TestOscWave::IsSameTrigger(OscWave* osc_wave) {
  TestOscWave* test_osc = static_cast<TestOscWave*>(osc_wave);
  return test_osc->trigger_.get() == trigger_.get();
}

WaveRange TestOscWave::trigger_offset_range() {
  return trigger_->offset_range;
}

double TestOscWave::trigger_offset() {
  return trigger_->offset;
}

bool TestOscWave::trigger_show() {
  return true;
}

bool TestOscWave::trigger_is_relate() {
  return true;
}

OscWave* TestOscWave::trigger_wave() {
  return trigger_->trigger_wave;
}

bool TestOscWave::IsSameVertical(OscWave* osc_wave) {
  TestOscWave* test_osc = static_cast<TestOscWave*>(osc_wave);
  return test_osc->vertical_.get() == vertical_.get();
}

int TestOscWave::vertical_div() {
  return vertical_->div;
}

int TestOscWave::vertical_window_size() {
  return vertical_->windows_size;
}

WaveRange TestOscWave::vertical_range() {
  return vertical_->range;
}

WaveRange TestOscWave::vertical_offset_range() {
  return vertical_->range;
}

double TestOscWave::vertical_offset() {
  return vertical_->offset;
}

bool TestOscWave::IsSameHorizontal(OscWave* osc_wave) {
  TestOscWave* test_osc = static_cast<TestOscWave*>(osc_wave);
  return test_osc->horiz_.get() == horiz_.get();
}

int TestOscWave::horizontal_div() {
  return horiz_->div;
}

int TestOscWave::horizontal_window_size() {
  return horiz_->windows_size;
}

WaveRange TestOscWave::horizontal_range() {
  return horiz_->range;
}

WaveRange TestOscWave::horizontal_offset_range() {
  return horiz_->range;
}

double TestOscWave::horizontal_offset() {
  return horiz_->offset;
}

void TestOscWave::DoCommand(int command_id) {
  NOTREACHED();
}

void TestOscWave::DoRangeCommand(int command_id, WaveRange range) {
  NOTREACHED();
}

string16 TestOscWave::name() {
  return name_;
}

SkColor TestOscWave::color() {
  return color_;
}

const gfx::Image& TestOscWave::icon() {
  return icon_;
}

SkColor TestOscWave::trigger_color() {
  return trigger_->trigger_wave->color();
}

SkColor TestOscWave::vertical_color() {
  return vertical_->color;
}

bool TestOscWave::vertical_show() {
  return true;
}

SkColor TestOscWave::horizontal_color() {
  return horiz_->color;
}

bool TestOscWave::horizontal_show() {
  return true;
}

void TestOscWave::OnNotifyChanged(int changed_part) {
  NotifyChanged(changed_part);
}

TestOscWave::~TestOscWave() {
  horiz_->RemoveObserver(this);
  trigger_->RemoveObserver(this);
}

void TestOscWave::AddHoriz(Horiz* horiz) {
  horiz_ = horiz;
  horiz_->AddObserver(this);
}

void TestOscWave::AddTrigger(Trigger* trig) {
  trigger_ = trig;
  trigger_->AddObserver(this);
}

void TestOscWave::AddVertical(Vertical* vertical) {
  vertical_ = vertical;
}


std::vector<TestOscWave*> CreateWaves() {
  std::vector<TestOscWave*> waves;

  Horiz* horiz = new Horiz();
  horiz->div = 14;
  horiz->offset = 0;
  horiz->windows_size = 14;
  horiz->range.begin = - 2000;
  horiz->range.end = 2000;
  horiz->color = SkColorSetRGB(125, 125, 0);

  Trigger* trig = new Trigger();
  trig->offset = 300;
  trig->offset_range.begin = 200;
  trig->offset_range.end = -200;

  {
  // can-h
  TestOscWave* wave = new TestOscWave;
  wave->name_ = L"CAN-H";
  wave->color_ = SkColorSetRGB(125, 0, 0);
  
  Vertical* vertical = new Vertical();
  vertical->div = 12;
  vertical->range.begin = 1000;
  vertical->range.end = -1000;
  vertical->windows_size = 12;
  vertical->offset = 0;
  vertical->color = wave->color_;
  wave->AddVertical(vertical);
  wave->AddHoriz(horiz);
  wave->AddTrigger(trig);
  waves.push_back(wave);

  TestAnaData* wave_data = new TestAnaData;
  wave_data->data_range_.begin = -1000;
  wave_data->data_range_.end = 1000;
  wave_data->data_.reserve(200);
  for (int j = 0; j < 200; j++) {
    wave_data->data_.push_back(sin(j*4*2*M_PI/50) * 500);
  }
  wave->wave_data_.reset(wave_data);

  trig->trigger_wave = wave;

  }

  {
  // can-l
  TestOscWave*  wave = new TestOscWave;
  wave->name_ = L"CAN-L";
  wave->color_ = SkColorSetRGB(0, 125, 0);
  Vertical* vertical = new Vertical();
  vertical->div = 16;
  vertical->range.begin = 2000;
  vertical->range.end = -2000;
  vertical->windows_size = 16;
  vertical->offset = 0;
  vertical->color = wave->color_;
  wave->AddVertical(vertical);
  wave->AddHoriz(horiz);
  wave->AddTrigger(trig);
  waves.push_back(wave);

  TestAnaData* wave_data = new TestAnaData;
  wave_data->data_range_.begin = -500;
  wave_data->data_range_.end = 500;
  wave_data->data_.reserve(200);
  for (int j = 0; j < 200; j++) {
    wave_data->data_.push_back(sin(j*4*2*M_PI/50) * 500 + 500);
  }
  wave->wave_data_.reset(wave_data);
  }

  {
  // can-diff
  TestOscWave* wave = new TestOscWave;
  wave->name_ = L"CAN-DIFF";
  wave->color_ = SkColorSetRGB(0, 0, 125);
  Vertical* vertical = new Vertical();
  vertical->div = 8;
  vertical->range.begin = 3000;
  vertical->range.end = -3000;
  vertical->windows_size = 8;
  vertical->offset = 0;
  vertical->color = wave->color_;

  wave->AddVertical(vertical);
  wave->AddHoriz(horiz);
  wave->AddTrigger(trig);
  waves.push_back(wave);

  TestAnaData* wave_data = new TestAnaData;
  wave_data->data_range_.begin = -1500;
  wave_data->data_range_.end = 1500;
  wave_data->data_.reserve(200);
  for (int j = 0; j < 200; j++) {
    wave_data->data_.push_back(sin(j*4*2*M_PI/50) * 500 - 500);
  }
  wave->wave_data_.reset(wave_data);
  }

  return waves;
}

