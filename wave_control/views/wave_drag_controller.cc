#include "wave_control/views/wave_drag_controller.h"

#include "ui/gfx/vector2d.h"
#include "ui/views/widget/widget.h"
#include "ui/base/events/event_utils.h"
 
#include "wave_control/views/wave_control_view.h"

using namespace views;

namespace {
static const int kDragThreshold = 15;
}

void WaveDragController::Init(YTWaveContainerView* start_view,
                              Wave* select_wave,
                              const std::vector<views::View*>& renderers, 
                              const std::vector<gfx::Rect>& renderer_bounds, 
                              const gfx::Rect& paint_bounds) {
  CHECK(!active_);
  start_view_ = start_view;
  renderers_ = renderers;
  select_wave_ = select_wave;
  renderer_bounds_ = renderer_bounds;
  paint_bounds_ = paint_bounds;
  canceling_ = false;
}


bool WaveDragController::DragEvent(const ui::MouseEvent& event) {
  return event.IsOnlyLeftMouseButton() && event.IsShiftDown();
}

bool WaveDragController::OnMousePressed(views::View* view, const ui::MouseEvent& event) {
  CHECK(!active_); 
  if (canceling_) {
    return false;
  }

  if (!DragEvent(event)) {
    return false;
  }

  CHECK(!pressed_);
  pressed_ = true;
  pressed_view_ = view;
  pressed_point_ = event.location();
  return true;
}

bool WaveDragController::OnMouseDragged(views::View* view, const ui::MouseEvent& event) {
  CHECK(pressed_);
  CHECK(view == pressed_view_);
  if (canceling_) {
    return false;
  }

  if (!active_) {
    gfx::Point point =  event.location();
    gfx::Vector2d v_2d = point - pressed_point_;
    if (v_2d.Length() >= kDragThreshold) {
      active_ = true;
      InitDrag();
    }
  } else {
    ContinueDrag();
  }

  return true;
}

void WaveDragController::OnMouseReleased(views::View* view, const ui::MouseEvent& event) {
  CHECK(pressed_);
  CHECK(view == pressed_view_);
  pressed_ = false;
  if (canceling_) {
    return;
  }

  if (active_) {
    Drop();
  }
}

void WaveDragController::OnMouseCaptureLost(views::View* view) {
  CHECK(view == pressed_view_);
  pressed_ = false;
  Cancel();
}

void WaveDragController::Cancel() {
  // already canceled
  if (canceling_) {
    return;
  }
  canceling_ = true;
  if (active_) {
    ResetDrag();
  }
}

void WaveDragController::InitDrag() {
  drag_view_.reset(new DraggedThumbView(renderers_, renderer_bounds_, paint_bounds_));
  UpdateIndicate();
}

void WaveDragController::ContinueDrag() {
  drag_view_->MoveTo(GetScreen()->GetCursorScreenPoint());
  UpdateIndicate();
}

void WaveDragController::Drop() {
  drag_view_.reset(NULL);
  control_view_->ShowDropIndicate(false);

  gfx::Point s_point = GetScreen()->GetCursorScreenPoint();
  views::View::ConvertPointFromScreen(control_view_, &s_point);
  bool is_view;
  size_t view_index;
  size_t insert_index;
  control_view_->GetIndicate(s_point, &is_view, &view_index, &insert_index);
  WaveControl* wave_control = control_view_->wave_control();
  if (is_view) {
    WaveContainer* container = wave_control->GetWaveContainerAt(view_index);
    bool ret = wave_control->CANMoveWaveTo(select_wave_, container);
    if (ret) {
      wave_control->MoveWaveTo(select_wave_, container);
    } 
  } else {
    bool ret = wave_control->CANCreateWaveContainerAt(select_wave_, insert_index);
    if (ret) {
      wave_control->CreateWaveContainerAt(select_wave_, insert_index);
    }
  }
}

void WaveDragController::ResetDrag() {
  drag_view_.reset(NULL);
  control_view_->ShowDropIndicate(false);
}

void WaveDragController::UpdateIndicate() {
  gfx::Point s_point = GetScreen()->GetCursorScreenPoint();
  View::ConvertPointFromScreen(control_view_, &s_point);
  bool is_view;
  size_t view_index;
  size_t insert_index;

  control_view_->GetIndicate(s_point, &is_view, &view_index, &insert_index);
  WaveControl* wave_control = control_view_->wave_control();
  if (is_view) {
    WaveContainer* container = wave_control->GetWaveContainerAt(view_index);
    bool ret = wave_control->CANMoveWaveTo(select_wave_, container);
    if (ret) {
      control_view_->ShowDropIndicate(true);
      control_view_->IndicateView(view_index);
    } else {
      control_view_->ShowDropIndicate(false);
    }
  } else {
    bool ret = wave_control->CANCreateWaveContainerAt(select_wave_, insert_index);
    if (ret) {
      control_view_->ShowDropIndicate(true);
      control_view_->IndicateGap(insert_index);
    } else {
      control_view_->ShowDropIndicate(false);
    }
  }
}

WaveDragController::WaveDragController(WaveControlView* control_view)
    : control_view_(control_view)
    , active_(false)
    , canceling_(false)
    , pressed_(false)
    , pressed_view_(NULL)
    , screen_(NULL) {
  
  // Listen for Esc key presses.
  MessageLoopForUI::current()->AddObserver(this);
}

WaveDragController::~WaveDragController() {
  MessageLoopForUI::current()->RemoveObserver(this);
}

base::EventStatus WaveDragController::WillProcessEvent(const base::NativeEvent& event) {
  return base::EVENT_CONTINUE;
}

void WaveDragController::DidProcessEvent(const base::NativeEvent& event) {
  if (ui::EventTypeFromNative(event) == ui::ET_KEY_PRESSED &&
      ui::KeyboardCodeFromNative(event) == ui::VKEY_ESCAPE) {
    Cancel();
  }
}

gfx::Screen* WaveDragController::GetScreen() {
  if (screen_ == NULL) {
    screen_ = gfx::Screen::GetScreenFor(
      control_view_->GetWidget()->GetNativeView());
  }
  return screen_;
}
