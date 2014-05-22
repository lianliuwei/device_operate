#include "wave_control/views/osc_wave_view.h"

namespace {
bool IsSet(int set, int test) {
  return (set & test) != 0;
}

}

void OscWaveView::OnOscWaveChanged(OscWave* osc_wave, int change_set) {
  int update_transform = OscWave::kVertical | OscWave::kVerticalOffset 
      | OscWave::kHorizontal | OscWave::kHorizontalOffset;
  if (IsSet(change_set, update_transform)) {
    UpdateTransform();
  } 
  if (IsSet(change_set, OscWave::kData)) {
    UpdateData();
  }
}


gfx::Transform OscWaveView::CalcTransform() {
  gfx::Transform transform;

  if (size() == gfx::Size(0, 0)) {
    return transform;
  }

  int y_window_size, x_window_size;
  double y_data_size, x_data_size;
  double y_offset, x_offset;
  {
  y_offset = osc_wave_->vertical_offset();
  WaveRange wave_range = osc_wave_->vertical_range();
  double div_size = (wave_range.end - wave_range.begin) / osc_wave_->vertical_div();
  y_data_size = div_size * osc_wave_->vertical_window_size();
  y_window_size = GetLocalBounds().height();
  }

  {
  x_offset = osc_wave_->horizontal_offset();
  WaveRange wave_range = osc_wave_->horizontal_range();
  double div_size = (wave_range.end - wave_range.begin) / osc_wave_->horizontal_div();
  x_data_size = div_size * osc_wave_->horizontal_window_size();
  x_window_size = GetLocalBounds().width();
  }

  transform.Translate(x_window_size / 2, y_window_size / 2);
  transform.Scale(x_window_size / x_data_size, y_window_size / y_data_size);
  transform.Translate(-x_offset, -y_offset);

  return transform;
}

void OscWaveView::UpdateTransform() {
  set_data_transform(CalcTransform());
}

gfx::Transform OscWaveView::GetDataTransform() {
  return CalcTransform();
}

void OscWaveView::UpdateData() {
  set_line_data(&(osc_wave_->Data()));
}

OscWaveView::OscWaveView(OscWave* osc_wave)
    : osc_wave_(osc_wave) {
  set_wave_color(osc_wave->color());
  // TODO may dist them
  set_dot_color(osc_wave->color());
  set_show_style(KLineAndAutoDot);

  osc_wave->AddObserver(this);
  UpdateData();
}

OscWaveView::~OscWaveView() {
  osc_wave_->RemoveObserver(this);
}

void OscWaveView::OnBoundsChanged(const gfx::Rect& previous_bounds) {
  UpdateTransform();
}

bool OscWaveView::OnMouseWheel(const ui::MouseWheelEvent& event) {
  if (event.IsControlDown()) {
    if (event.y_offset() > 0 ) {
      osc_wave_->ZoomInX();
    } else {
      osc_wave_->ZoomOutX();
    }
  }
  if (event.IsShiftDown()) {
    if (event.y_offset() > 0 ) {
      osc_wave_->ZoomInY();
    } else {
      osc_wave_->ZoomOutY();
    }
  }
  if (!event.IsShiftDown() && !event.IsControlDown()) {
    double offset = osc_wave_->horizontal_offset();
    WaveRange range = osc_wave_->horizontal_range();
    double div_value = (range.end - range.begin) / osc_wave_->horizontal_div();
    int div = offset  / div_value;
    if (event.y_offset() < 0) {
      div -= 1;
    } else {
      div += 1;
    }
    osc_wave_->MoveToX(div * div_value);
  }

  return true;
}
