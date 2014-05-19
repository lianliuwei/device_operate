#include "wave_control/views/wave_control_view.h"

#include "base/debug/trace_event.h"
#include "ui/gfx/canvas.h"
#include "third_party/skia/include/core/SkPaint.h"

#include "wave_control/views/wave_control_view_factory.h"
#include "wave_control/views/wave_control_views_constants.h"

using namespace views;

namespace {
// view id
static const int kFrontSize = 2;
static const int kWaveIndicateViewID = 0;
static const int kGapIndicateViewID = 1;

class ViewIndicate : public views::View {
public:
  ViewIndicate() {}
  virtual ~ViewIndicate() {}

  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE {
    SkPaint paint;
    paint.setColor(SkColorSetARGB(200, 255, 255, 255));
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(SkIntToScalar(4));

    canvas->DrawRoundRect(GetLocalBounds(), 20, paint);
  }

  DISALLOW_COPY_AND_ASSIGN(ViewIndicate);
};

class GapIndicate : public views::View {
public:
  GapIndicate() {}
  virtual ~GapIndicate() {}

  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE {
    SkPaint paint;
    paint.setColor(SkColorSetARGB(200, 255, 255, 255));
    paint.setStrokeWidth(SkIntToScalar(5));

    canvas->DrawLine(gfx::Point(0, height()/2), gfx::Point(width(), height()/2), paint);
  }
  DISALLOW_COPY_AND_ASSIGN(GapIndicate);
};

}

// just Vertical Layout the ContainerView
WaveControlView::WaveControlView(WaveControl* wave_control)
    : wave_control_(wave_control)
    , drag_controller_(this)
    , show_indicate_(false)
    , view_index_(-1)
    , gap_index_(-1)
    , view_indiciate_(NULL)
    , gap_indicate_(NULL)
    , inited_(false) {
  set_background(Background::CreateSolidBackground(kWaveControlBackgroundColor));
  
  view_indiciate_ = new ViewIndicate();
  view_indiciate_->SetVisible(false);
  AddChildViewAt(view_indiciate_, 0);
  gap_indicate_ = new GapIndicate();
  gap_indicate_->SetVisible(false);
  AddChildViewAt(gap_indicate_, 1);

}

WaveControlView::~WaveControlView() {
  if (inited_) {
    wave_control()->RemoveWaveContainerObserver(this);
  }
}


void WaveControlView::ViewHierarchyChanged(const ViewHierarchyChangedDetails& details) {
  if (details.is_add && GetWidget() && !inited_) {
    inited_ = true;

    wave_control_->AddWaveContainerObserver(this);
    // fetch WaveContainer
    ListItemsAdded(0, wave_control_->WaveContainerCount());
  }
}

void WaveControlView::ListItemsAdded(size_t start, size_t count) {
  for (size_t i = 0; i < count; ++i) {
    WaveContainer* container = wave_control_->GetWaveContainerAt(start + i);
    View* view = WaveControlViewFactory::GetInstance()->Create(container, this);
    this->AddChildViewAt(view, ViewID(start + i));
  }
  Layout();
}

void WaveControlView::ListItemsRemoved(size_t start, size_t count) {
  View::Views need_remove;
  need_remove.reserve(count);
  for (size_t i = 0; i < count; ++i) {
    need_remove.push_back(this->child_at(ViewID(start + i)));
  }
  for (size_t i = 0; i < need_remove.size(); ++i) {
    this->RemoveChildView(need_remove[i]);
    delete need_remove[i];
  }
  Layout();
}

void WaveControlView::ListItemMoved(size_t index, size_t target_index) {
  this->ReorderChildView(this->child_at(ViewID(index)), 
        ViewID(target_index));
}

void WaveControlView::ListItemsChanged(size_t start, size_t count) {
  ListItemsRemoved(start, count);
  ListItemsAdded(start, count);
}


// if layout more complex this need to move to LayoutManager
void WaveControlView::GetIndicate(const gfx::Point& point, 
                                  bool* is_view, size_t* view_index, size_t* gap_index) {
  int y = point.y();
  int child_num = ContainerViewCount();
  if (child_num == 0) {
    *is_view = false;
    *gap_index = 0;
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
    *gap_index = child_index;
  } else if (offset > c_height - 8) {
    *is_view = false;
    *gap_index = child_index + 1;
  } else {
    *is_view = true;
    *view_index = child_index;
    DCHECK(child_index < ContainerViewCount());
  }

}

void WaveControlView::ShowDropIndicate(bool show) {
  show_indicate_ = show;
  if (!show) {
    view_indiciate_->SetVisible(false);
    gap_indicate_->SetVisible(false);
  }
}

void WaveControlView::IndicateView(size_t i) {
  if (show_indicate_) {
    gap_indicate_->SetVisible(false);
    view_indiciate_->SetVisible(true);
    LayoutViewIndicate(i);
  }
  indicate_is_view_ = true;
  view_index_ = i;
}

void WaveControlView::IndicateGap(size_t i) {
  if (show_indicate_) {
    view_indiciate_->SetVisible(false);
    gap_indicate_->SetVisible(true);
    LayoutGapIndicate(i);
  }
  indicate_is_view_ = false;
  gap_index_ = i;
}


void WaveControlView::Layout() {
  gfx::Rect child_area(GetLocalBounds());
  child_area.Inset(GetInsets());

  int x = child_area.x();
  int y = child_area.y();
  for (int i = 2; i < child_count(); ++i) {
    int view_need = child_area.height() / ContainerViewCount();
    View* child = child_at(i);
    gfx::Rect bounds(x, y, child_area.width(), child_area.height());
    bounds.set_height(view_need);
    y += view_need;

    child->SetBoundsRect(bounds);
  }

  if (show_indicate_) {
    if (indicate_is_view_) {
      LayoutViewIndicate(view_index_);
    } else {
      LayoutGapIndicate(gap_index_);
    }
  }
}

// paint indicate on top.
void WaveControlView::PaintChildren(gfx::Canvas* canvas) {
  TRACE_EVENT0("views", "View::PaintChildren");
  for (int count = child_count(), i = count - 1; i >= 0; --i) {
    if (!child_at(i)->layer())
      child_at(i)->Paint(canvas);
  }
}


void WaveControlView::LayoutViewIndicate(size_t i) {
  DCHECK(ContainerViewCount() > 0);
  int c_height = height() / ContainerViewCount();
  gfx::Rect i_rect = GetLocalBounds();
  i_rect.set_height(c_height);
  i_rect.set_y(i * c_height);
  i_rect.Inset(gfx::Insets(2, 2, 2, 2));
  view_indiciate_->SetBoundsRect(i_rect);
}

void WaveControlView::LayoutGapIndicate(size_t i) {
  DCHECK(ContainerViewCount() > 0);
  int c_height = height() / ContainerViewCount();
  gfx::Rect i_rect = GetLocalBounds();
  i_rect.set_height(7);
  i_rect.set_y(i * c_height - 3);
  i_rect.Inset(gfx::Insets(0, 2, 0, 2));
  gap_indicate_->SetBoundsRect(i_rect);
}

int WaveControlView::ViewID(int wave_container_id) {
  return wave_container_id + kFrontSize;
}

int WaveControlView::ContainerViewCount() const {
  return child_count() - kFrontSize;
}

