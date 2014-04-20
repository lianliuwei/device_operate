#include "wave_control/views/simple_ana_wave_view.h"
#include "wave_control/views/transform_util.h"

namespace {
bool IsSet(int set, int test) {
  return (set & test) != 0;
}

}

void SimpleAnaWaveView::OnSimpleAnaWaveChanged(SimpleAnaWave* ana_wave, int change_set) {
  int update_transform = SimpleAnaWave::kVertical | SimpleAnaWave::kHorizontal;
  if (IsSet(change_set, update_transform)) {
    UpdateTransform();
  }
  if (IsSet(change_set, SimpleAnaWave::kData)) {
    UpdateData();
  }
}

void SimpleAnaWaveView::UpdateTransform() {
  gfx::Transform transform;

  int y_window_size, x_window_size;
  double y_data_size, x_data_size;
  double y_offset, x_offset;
  {
  WaveRange wave_range = ana_wave_->vertical_range();
  y_data_size = wave_range.end - wave_range.begin;
  y_offset = wave_range.begin;
  y_window_size = GetLocalBounds().height();
  }

  {
  WaveRange wave_range = ana_wave_->horizontal_range();
  x_data_size = wave_range.end - wave_range.begin;
  x_offset = wave_range.begin;
  x_window_size = GetLocalBounds().width();
  }

  transform.Scale(x_window_size / x_data_size, y_window_size / y_data_size);
  transform.Translate(-x_offset, -y_offset);

  set_data_transform(transform);
}

void SimpleAnaWaveView::UpdateData() {
  set_line_data(&(ana_wave_->Data()));
  handle_offset_ = (ana_wave_->Data().MaxY() + ana_wave_->Data().MinY()) / 2;
}

SimpleAnaWaveView::SimpleAnaWaveView(SimpleAnaWave* ana_wave)
    : ana_wave_(ana_wave) {
  set_wave_color(ana_wave->color());
  set_dot_color(ana_wave->color());
  set_show_style(KLineAndAutoDot);

  ana_wave->AddObserver(this);
  UpdateData();
}

SimpleAnaWaveView::~SimpleAnaWaveView() {
  ana_wave_->RemoveObserver(this);
}

void SimpleAnaWaveView::OnBoundsChanged(const gfx::Rect& previous_bounds) {
  UpdateTransform();
}

int SimpleAnaWaveView::GetYOffset() {
  return TransformY(data_transform(), handle_offset_);
}

void SimpleAnaWaveView::MoveToY(int y_pos) {
  int logic_offset = TransformReverseY(data_transform(), y_pos);
  int move = handle_offset_ - logic_offset;
  WaveRange range = ana_wave_->vertical_range();
  range.begin += move;
  range.end += move;
  ana_wave_->set_vertical_range(range);
}
