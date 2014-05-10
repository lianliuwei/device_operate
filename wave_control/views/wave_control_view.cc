#include "wave_control/views/wave_control_view.h"

#include "ui/gfx/canvas.h"
#include "third_party/skia/include/core/SkPaint.h"

#include "wave_control/views/wave_control_view_factory.h"
#include "wave_control/views/wave_control_views_constants.h"
#include "wave_control/views/fill_box_layout.h"

using namespace views;

// just Vertical Layout the ContainerView
WaveControlView::WaveControlView(WaveControl* wave_control)
    : wave_control_(wave_control)
    , drag_controller_(this) {
  set_background(Background::CreateSolidBackground(kWaveControlBackgroundColor));
  SetLayoutManager(new FillBoxLayout(FillBoxLayout::kVertical, 0, 0, 0));
  wave_control->AddWaveContainerObserver(this);

  // fetch WaveContainer
  ListItemsAdded(0, wave_control->WaveContainerCount());
}

WaveControlView::~WaveControlView() {
  wave_control()->RemoveWaveContainerObserver(this);
}

void WaveControlView::ListItemsAdded(size_t start, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    WaveContainer* container = wave_control_->GetWaveContainerAt(start + i);
    View* view = WaveControlViewFactory::GetInstance()->Create(container, this);
    this->AddChildViewAt(view, start + i);
  }
}

void WaveControlView::ListItemsRemoved(size_t start, size_t count) {
  View::Views need_remove;
  need_remove.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    need_remove.push_back(this->child_at(start + i));
  }
  for (size_t i = 0; i < need_remove.size(); ++i) {
    this->RemoveChildView(need_remove[i]);
    delete need_remove[i];
  }
}

void WaveControlView::ListItemMoved(size_t index, size_t target_index) {
  this->ReorderChildView(this->child_at(index), target_index);
}

void WaveControlView::ListItemsChanged(size_t start, size_t count) {
  ListItemsRemoved(start, count);
  ListItemsAdded(start, count);
}


// if layout more complex this need to move to LayoutManager
void WaveControlView::GetIndicate(const gfx::Point& point, 
                                  bool* is_view, size_t* i, size_t* index) {
  int y = point.y();
  int child_num = child_count();
  if (child_num == 0) {
    *is_view = false;
    *index = 0;
    return;
  }
  if (y < 0) {
    y = 0;
  } else if (y >= height()) {
    y = height() - 1;
  } 

  int c_height = height() / child_num;
  int child_index = y / c_height;
  int offset = y % c_height;
  if (offset < 8) {
    *is_view = false;
    *index = child_index;
  } else if (offset > c_height - 8) {
    *is_view = false;
    *index = child_index + 1;
  } else {
    *is_view = true;
    *i = child_index;
    DCHECK(child_index < child_count());
  }

}

void WaveControlView::ShowDropIndicate(bool show) {
  show_indicate_ = show;
  SchedulePaint();
}

void WaveControlView::IndicateView(size_t i) {
  indicate_is_view_ = true;
  view_index_ = i;
}

void WaveControlView::IndicateGap(size_t index) {
  indicate_is_view_ = false;
  insert_index_ = index;
}

void WaveControlView::PaintChildren(gfx::Canvas* canvas) {
  View::PaintChildren(canvas);
  PaintIndicate(canvas);
}

void WaveControlView::PaintIndicate(gfx::Canvas* canvas) {
  if (!show_indicate_) {
    return;
  }
  if (child_count() == 0) {
    return;
  }
  SkPaint paint;
  paint.setColor(SkColorSetARGB(200, 255, 255, 255));
  paint.setAntiAlias(true);
  paint.setStrokeWidth(SkIntToScalar(4));

  int c_height = height() / child_count();
  if (indicate_is_view_) {
    gfx::Rect i_rect = GetLocalBounds();
    i_rect.set_height(c_height);
    i_rect.set_y(view_index_ * c_height);
    i_rect.Inset(gfx::Insets(2, 2, 2, 2));
    canvas->DrawRoundRect(i_rect, 90, paint);
  } else {
    gfx::Point start_point(2, insert_index_ * c_height);
    gfx::Point end_point(width() - 2, insert_index_ * c_height);

    canvas->DrawLine(start_point, end_point, paint);
  }
}

