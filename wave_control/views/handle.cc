#include "wave_control/views/handle.h"

#include "ui/gfx/image/image.h"
#include "wave_control/views/handle_bar.h"

using namespace views;

// model config is set by the handleBar
Handle::Handle(HandleBar* bar)
  : TextButton(NULL, L"")
  , mouse_offset_(-1)
  , bar_(bar) {
  DCHECK(bar_) << "the Handle need HandleBar object.";
  set_border(NULL); // no need border
}

bool Handle::OnMousePressed(const ui::MouseEvent& event) {
  if (!event.IsOnlyLeftMouseButton()) {
    return false;
  }
  bool ret = TextButton::OnMousePressed(event);
  mouse_offset_ = bar_->IsHorizontal() ? event.x() : event.y();
  bar_->ActiveHandle(tag());
  int offset = bar_->IsHorizontal() ? x() : y();
  bar_->OnHandlePressed(tag(), offset);
  return ret;
}

bool Handle::OnMouseDragged(const ui::MouseEvent& event) {
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
  bar_->OnHandleReleased(tag());
}

void Handle::OnMouseCaptureLost() {
  bar_->OnHandleReleased(tag());
}




