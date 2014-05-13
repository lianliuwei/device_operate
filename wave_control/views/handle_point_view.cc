#include "wave_control/views/handle_point_view.h"

#include "ui/gfx/canvas.h"

#include "wave_control/views/wave_control_views_constants.h"

using namespace ui;
using namespace gfx;

void HandlePointView::OnHandlePressed(int id, int offset, bool horiz) {
  DCHECK(in_drag_ == false);
  in_drag_ = true;
  offset_ = offset;
  horiz_ = horiz;
  animation_.Stop();
  SchedulePaint();
}

void HandlePointView::OnHandleMove(int ID, int offset) {
  DCHECK(in_drag_ == true);
  offset_ = offset;
  animation_.Stop();
  SchedulePaint();
}

void HandlePointView::OnHandleReleased(int id) {
  DCHECK(in_drag_ == true);
  in_drag_ = false;
  animation_.Start();
  SchedulePaint();
}

void HandlePointView::OnPaint(gfx::Canvas* canvas) {
  if (!in_drag_ && !animation_.is_animating()) {
    return;
  }
  SkPaint paint;
  paint.setStrokeWidth(1);
  if (in_drag_) {
    paint.setColor(kHandlePointColor);
  } else if (animation_.is_animating()) {
    uint8 a_value = animation_.CurrentValueBetween(0xFF, 0x00);
    SkColor color = SkColorSetA(kHandlePointColor, a_value);
    paint.setColor(color);
  }
  if (horiz_) {
    canvas->DrawLine(Point(offset_, 0), Point(offset_, height()), paint);
  } else {
    canvas->DrawLine(Point(0, offset_), Point(width(), offset_), paint);
  }
}

HandlePointView::HandlePointView()
    : in_drag_(false)
    , offset_(0)
    , horiz_(true)
    , animation_(30, this) {
  animation_.SetDuration(1000);
}

void HandlePointView::AnimationEnded(const Animation* animation) {
  SchedulePaint();
}

void HandlePointView::AnimationProgressed(const Animation* animation) {
  SchedulePaint();
}

void HandlePointView::AnimationCanceled(const Animation* animation) {
  SchedulePaint();
}
