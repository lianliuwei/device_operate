#pragma once

#include <vector>

#include "base/message_loop.h"
#include "ui/gfx/screen.h"

#include "wave_control/views/dragged_thumb_view.h"

class WaveControlView;
class YTWaveContainerView;
class Wave;

class WaveDragController : public MessageLoopForUI::Observer {
public:
  WaveDragController(WaveControlView* control_view);
  virtual ~WaveDragController();

  bool active() const { return active_; }

  // data for init drag.
  void Init(YTWaveContainerView* start_view, 
            Wave* select_wave,
            const std::vector<views::View*>& renderers,
            const std::vector<gfx::Rect>& renderer_bounds,
            const gfx::Rect& paint_bounds);

  void Cancel();
  // update thumb view.
  void Update();

  bool DragEvent(const ui::MouseEvent& event);
  bool OnMousePressed(views::View* view, const ui::MouseEvent& event);
  bool OnMouseDragged(views::View* view, const ui::MouseEvent& event);
  void OnMouseReleased(views::View* view, const ui::MouseEvent& event);
  void OnMouseCaptureLost(views::View* view);

  YTWaveContainerView* start_view() { return start_view_; }

private:
  void InitDrag();
  void ContinueDrag();
  void Drop();
  void ResetDrag();

  gfx::Screen* GetScreen();

  void UpdateIndicate();

  // Overridden from MessageLoop::Observer:
  virtual base::EventStatus WillProcessEvent(
      const base::NativeEvent& event) OVERRIDE;
  virtual void DidProcessEvent(const base::NativeEvent& event) OVERRIDE;

  bool active_;
  bool canceling_;

  bool pressed_;
  views::View* pressed_view_;
  gfx::Point pressed_point_;

  // data for init drag
  YTWaveContainerView* start_view_; 
  Wave* select_wave_;
  std::vector<views::View*> renderers_;
  std::vector<gfx::Rect> renderer_bounds_;
  gfx::Rect paint_bounds_;

  scoped_ptr<DraggedThumbView> drag_view_;

  WaveControlView* control_view_;

  gfx::Screen* screen_;

  DISALLOW_COPY_AND_ASSIGN(WaveDragController);
};