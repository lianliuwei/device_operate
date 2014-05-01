#include "wave_control/views/measure_line/measure_line_part_view.h"

#include "ui/gfx/point.h"
#include "ui/gfx/canvas.h"
#include "third_party/skia/include/core/SkPaint.h"

#include "wave_control/views/wave_control_views_constants.h"

using namespace gfx;

void MeasureLinePartView::OnPaint(gfx::Canvas* canvas) {
  Size v_size = size();
  SkPaint paint;
  paint.setStyle(SkPaint::kStroke_Style);
  paint.setStrokeWidth(SkIntToScalar(1));
  paint.setColor(kMeasureLineColor);
  if (horiz_) {
    int y = (v_size.height() + 1) / 2 - 1;
    canvas->DrawLine(Point(0, y), Point(v_size.width(), y), paint);
  } else {
    int x = (v_size.width() + 1) /2 - 1;
    canvas->DrawLine(Point(x, 0), Point(x, v_size.height()), paint);
  }
}

bool MeasureLinePartView::OnMousePressed(const ui::MouseEvent& event) {
  start_point_ = event.location();
  if (delegate_) {
    delegate_->OnPosChanged(this, line_point());
  }
  return true;
}

bool MeasureLinePartView::OnMouseDragged(const ui::MouseEvent& event) {
  gfx::Point current_point = event.location();
  if (horiz_) {
    int delta_y = current_point.y() - start_point_.y();
    Rect rect = bounds();
    rect.Offset(0, delta_y);
    SetBoundsRect(rect);
    other_pos_ = current_point.x();
  } else {
    int delta_x = current_point.x() - start_point_.x();
    Rect rect = bounds();
    rect.Offset(delta_x, 0);
    SetBoundsRect(rect);
    other_pos_ = current_point.y();
  }
  if (delegate_) {
    delegate_->OnPosChanged(this, line_point());
  }
  return true;
}

void MeasureLinePartView::OnMouseReleased(const ui::MouseEvent& event) {
  if (delegate_) {
    delegate_->OnPosChanged(this, line_point());
  }
}

int MeasureLinePartView::line_point() const {
  Rect rect = bounds();
  if (horiz_) {
    return rect.y() + (rect.height()+1)/2 - 1;
  } else {
    return rect.x() + (rect.width()+1)/2 - 1;
  }
}

gfx::Rect MeasureLinePartView::RectForPoint(int point, gfx::Rect parent_rect) {
  gfx::Rect rect = parent_rect;
  if (horiz_) {
    rect.set_height(kMesaureLineSize);
    int y = point - (kMesaureLineSize + 1) / 2 - 1;
    rect.set_y(y);
  } else {
    rect.set_width(kMesaureLineSize);
    int x = point - (kMesaureLineSize + 1) / 2 - 1;
    rect.set_x(x);
  }
  return rect;
}

MeasureLinePartView::MeasureLinePartView(bool horiz, Delegate* delegate)
    : horiz_(horiz)
    , delegate_(delegate) {

}

gfx::NativeCursor MeasureLinePartView::GetCursor(const ui::MouseEvent& event) {
  static HCURSOR kHorizArrow = LoadCursor(NULL, IDC_SIZENS);
  static HCURSOR KVerticalArrow = LoadCursor(NULL, IDC_SIZEWE);
  if (horiz_) {
    return kHorizArrow;
  } else {
    return KVerticalArrow;
  }
}
