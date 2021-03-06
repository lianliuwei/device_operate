#pragma once

#include "ui/views/controls/button/text_button.h"

class HandleBar;

// for user to drag and move.
// use text_button to show the text and icon
class Handle : public views::TextButton {
public:
  Handle(HandleBar* bar);
  virtual ~Handle() {};

  // overridden from views::View
  virtual gfx::Size GetMinimumSize() OVERRIDE;

  // overridden from views::View
  virtual bool OnMousePressed(const ui::MouseEvent& event) OVERRIDE;
  virtual bool OnMouseDragged(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseReleased(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseCaptureLost() OVERRIDE;
  virtual void VisibilityChanged(View* starting_from, bool is_visible) OVERRIDE;

private:
  void HandleReleased();

  HandleBar* bar_;
  bool in_drag_;
  bool press_called_;
  // The position of the mouse on the Handle axis relative to the top of this
  // view when the drag started.
  int mouse_offset_;
};
