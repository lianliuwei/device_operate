#pragma once

#include "ui/views/view.h"
#include "wave_control/views/handle_bar_observer.h"

class HandlePointView : public views::View 
                      , public HandleBarObserver {
public:
  HandlePointView();
  virtual ~HandlePointView() {}

  // overridden views::View
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;

  // implement HandleBarObserver
  virtual void OnHandlePressed(int id, int offset, bool horiz);
  virtual void OnHandleMove(int ID, int offset);
  virtual void OnHandleReleased(int id);
  virtual void OnHandleActive(int ID) {}

private:
  bool in_drag_;
  int offset_;
  // bar horiz
  bool horiz_;
};