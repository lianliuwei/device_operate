#include "wave_control/views/simple_ana_wave_view.h"
#include "wave_control/views/transform_util.h"

#include "wave_control/views/yt_wave_container_inner_view.h"

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


gfx::Transform SimpleAnaWaveView::CalcTransform() {
  gfx::Transform transform;

  if (size() == gfx::Size(0, 0)) {
    return transform;
  }

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

  return transform;
}

void SimpleAnaWaveView::UpdateTransform() {
  set_data_transform(CalcTransform());
}

gfx::Transform SimpleAnaWaveView::GetDataTransform() {
  return CalcTransform();
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
  return YInt(data_transform(), handle_offset_);
}

void SimpleAnaWaveView::MoveToY(int y_pos) {
  double logic_offset = ReverseYDouble(data_transform(), y_pos);
  double move = handle_offset_ - logic_offset;
  WaveRange range = ana_wave_->vertical_range();
  range.begin += move;
  range.end += move;
  ana_wave_->set_vertical_range(range);
}

bool SimpleAnaWaveView::OnMousePressed(const ui::MouseEvent& event) {
  if (ContainerEvent(event)) {
    return false;
  }

  if (!event.IsOnlyLeftMouseButton()) {
    return false;
  }
  start_point_ = event.location();
  drag_transform_ = data_transform();
  drag_v_range_ = ana_wave_->vertical_range();
  drag_h_range_ = ana_wave_->horizontal_range();
  return true;
}

bool SimpleAnaWaveView::OnMouseDragged(const ui::MouseEvent& event) {
  gfx::Point start = start_point_;
  gfx::Point now = event.location();
  double offset_x = ReverseXDouble(drag_transform_, now.x()) 
      - (ReverseXDouble(drag_transform_, start.x()));
  double offset_y = ReverseYDouble(drag_transform_, now.y()) 
      - ReverseYDouble(drag_transform_, start.y());

  WaveRange v_range = drag_v_range_;
  v_range.begin += -offset_y;
  v_range.end += -offset_y;
  WaveRange h_range = drag_h_range_;
  h_range.begin += -offset_x;
  h_range.end += -offset_x;

  ana_wave_->set_vertical_range(v_range);
  ana_wave_->set_horizontal_range(h_range);
  return true;
}

void SimpleAnaWaveView::OnMouseReleased(const ui::MouseEvent& event) {

}

bool SimpleAnaWaveView::ContainerEvent(const ui::MouseEvent& event) {
  return static_cast<YTWaveContainerInnerView*>(parent())->ContainerEvent(event);
}
