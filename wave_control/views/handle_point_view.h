#pragma once

#include "ui/views/view.h"
#include "wave_control/views/handle_bar_observer.h"
#include "ui/base/animation/animation_delegate.h"
#include "ui/base/animation/linear_animation.h"

class HandlePointView : public views::View 
                      , public HandleBarObserver
                      , public ui::AnimationDelegate {
public:
  HandlePointView();
  virtual ~HandlePointView() {}

  // overridden views::View
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
  virtual bool HitTestRect(const gfx::Rect& rect) const { return false; }

  // implement HandleBarObserver
  virtual void OnHandlePressed(int id, int offset, bool horiz);
  virtual void OnHandleMove(int ID, int offset);
  virtual void OnHandleReleased(int id);
  virtual void OnHandleActive(int ID) {}

private:
  // implement ui::AnimationDelegate
  virtual void AnimationEnded(const ui::Animation* animation);
  virtual void AnimationProgressed(const ui::Animation* animation);
  virtual void AnimationCanceled(const ui::Animation* animation);

  bool in_drag_;
  int offset_;
  // bar horiz
  bool horiz_;

  ui::LinearAnimation animation_;
};