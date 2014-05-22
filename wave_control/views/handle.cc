#include "wave_control/views/handle.h"

#include "base/bind.h"

#include "ui/gfx/image/image.h"
#include "wave_control/views/handle_bar.h"

using namespace base;
using namespace views;

// model config is set by the handleBar
Handle::Handle(HandleBar* bar)
  : TextButton(NULL, L"")
  , mouse_offset_(-1)
  , bar_(bar)
  , in_drag_(false) {
  DCHECK(bar_) << "the Handle need HandleBar object.";
  set_border(NULL); // no need border
}

bool Handle::OnMousePressed(const ui::MouseEvent& event) {
  if (!event.IsOnlyLeftMouseButton()) {
    return false;
  }
  DCHECK(in_drag_ == false);
  in_drag_ = true;
  press_called_ = false;

  bool ret = TextButton::OnMousePressed(event);
  mouse_offset_ = bar_->IsHorizontal() ? event.x() : event.y();
  bar_->ActiveHandle(tag());
  int offset = bar_->IsHorizontal() ? x() : y();

  if (in_drag_) {
    press_called_ = true;
    bar_->OnHandlePressed(tag(), offset);
  }

  return ret;
}

bool Handle::OnMouseDragged(const ui::MouseEvent& event) {
  DCHECK(in_drag_);
  // notify the bar_ to move the handle. the bar_ calculate the handle represent
  // offset and set the model, model set the offset by it's rule(for example only odd)
  // model notify the handleBar to update, finish the handle be moved.
  if (bar_->IsHorizontal()) {
    int handle_x = x() + event.x() - mouse_offset_;
    bar_->OnHandleMove(tag(), handle_x);
  } else {
    int handle_y = y() + event.y() - mouse_offset_;
    bar_->OnHandleMove(tag(), handle_y);
  }
  return true;
}

gfx::Size Handle::GetMinimumSize() {
  gfx::ImageSkia image = icon();
  gfx::Size size = image.size();
  gfx::Size old_size = TextButton::GetMinimumSize();
  size.set_width(size.width() + old_size.width());
  size.set_height(std::max(size.height(), old_size.height()));
  return size;
}

void Handle::OnMouseReleased(const ui::MouseEvent& event) {
  DCHECK(in_drag_);
  in_drag_ = false;
  bar_->OnHandleReleased(tag());
}

void Handle::OnMouseCaptureLost() {
  DCHECK(in_drag_);
  in_drag_ = false;
  bar_->OnHandleReleased(tag());
}

void Handle::HandleReleased() {
  DCHECK(in_drag_);
  in_drag_ = false;
  bar_->OnHandleReleased(tag());
}

// HACK when after OnMoursePressed() the view may no be call OnMouseReleased()
// or OnMouseCaptureLost() when View become invisible
void Handle::VisibilityChanged(View* starting_from, bool is_visible) {
  if (!is_visible && in_drag_) {
    if (press_called_) {
      MessageLoop::current()->PostTask(FROM_HERE, Bind(&Handle::HandleReleased, 
        Unretained(this)));
    } else {
      in_drag_ = false;
    }
  }
}
