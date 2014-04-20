#include "wave_control/views/handle_point_view.h"

#include "ui/gfx/canvas.h"

#include "wave_control/views/wave_control_views_constants.h"

using namespace gfx;

void HandlePointView::OnHandlePressed(int id, int offset, bool horiz) {
  DCHECK(in_drag_ == false);
  in_drag_ = true;
  offset_ = offset;
  horiz_ = horiz;
  SchedulePaint();
}

void HandlePointView::OnHandleMove(int ID, int offset) {
  offset_ = offset;
  SchedulePaint();
}

void HandlePointView::OnHandleReleased(int id) {
  DCHECK(in_drag_ == true);
  in_drag_ = false;
  SchedulePaint();
}

void HandlePointView::OnPaint(gfx::Canvas* canvas) {
  if (!in_drag_) {
    return;
  }
  SkPaint paint;
  paint.setStrokeWidth(1);
  paint.setColor(kHandlePointColor);
  if (horiz_) {
    canvas->DrawLine(Point(offset_, 0), Point(offset_, height()), paint);
  } else {
    canvas->DrawLine(Point(0, offset_), Point(width(), offset_), paint);
  }
}

HandlePointView::HandlePointView()
    : in_drag_(false)
    , offset_(0)
    , horiz_(true) {
}
