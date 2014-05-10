#pragma once

#include "ui/views/view.h"
#include "ui/base/models/list_model_observer.h"

#include "wave_control/wave_control.h"
#include "wave_control/views/wave_drag_controller.h"

class WaveControlView : public views::View
                      , public ui::ListModelObserver {
public:
  WaveControlView(WaveControl* wave_control);
  virtual ~WaveControlView();

  WaveControl* wave_control() { return wave_control_; }
  
  WaveDragController* drag_controller() { return &drag_controller_; }

  // for wave_drag_controller
  void ShowDropIndicate(bool show);
  bool IsShowDropIndicate() const { return show_indicate_; }
  void IndicateView(size_t i);
  void IndicateGap(size_t index);

  void GetIndicate(const gfx::Point& point, bool* is_view, size_t* i, size_t* index);

  // override views::View
  virtual void PaintChildren(gfx::Canvas* canvas) OVERRIDE;

private:
  void PaintIndicate(gfx::Canvas* canvas);

  friend class WaveControlViewFactory;

  // implement ui::ListModelObserver
  virtual void ListItemsAdded(size_t start, size_t count);
  virtual void ListItemsRemoved(size_t start, size_t count);
  virtual void ListItemMoved(size_t index, size_t target_index);
  virtual void ListItemsChanged(size_t start, size_t count);

  WaveControl* wave_control_;

  bool show_indicate_;
  bool indicate_is_view_;
  size_t view_index_;
  size_t insert_index_;

  WaveDragController drag_controller_;

  DISALLOW_COPY_AND_ASSIGN(WaveControlView);
};
