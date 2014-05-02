#include "wave_control/views/measure_line/single_line_view.h"

#include "base/stringprintf.h"
#include "ui/gfx/canvas.h"
#include "third_party/skia/include/core/SkPaint.h"

#include "wave_control/views/transform_util.h"
#include "wave_control/views/wave_control_views_constants.h"
#include "wave_control/views/measure_line/measure_line_container_view.h"

using namespace base;
using namespace gfx;
using namespace views;

void SingleLineView::Layout() {
  if (size() == Size(0, 0)) {
    return;
  }

  int point; 
  if (state_ == kInitNoWave) {
    Size v_size = size();
    point = horiz_ ? v_size.width()*3/8 : v_size.height()*3/8; 

  } else if (state_ == kHadWave) {
    point = horiz_ ? TransformX(transform_, pos_) : TransformY(transform_, pos_);

  } else if (state_ == kNoWave) {
    point = line_->line_point();

  } else {
    NOTREACHED();
  }

  gfx::Rect rect = line_->RectForPoint(point, GetLocalBounds());
  line_->SetBoundsRect(rect);

  LayoutLabel(point);
}

SingleLineView::SingleLineView(bool horiz)
    : horiz_(horiz)
    , state_(kInitNoWave)
    , pos_(0)
    , wave_(NULL)
    , pos_label_(NULL)
    , value_label_(NULL) {
  line_ = new MeasureLinePartView(!horiz, this);
  AddChildView(line_);
}

void SingleLineView::OnPaint(gfx::Canvas* canvas) {
  if (state_ == kInitNoWave) {
  } else if (state_ == kHadWave) {
    if (!horiz_ || !has_value_) {
      return;
    }
    SkPaint paint;
    paint.setStrokeWidth(3);
    paint.setColor(kMeasureLineColor);
    int y_pos = TransformY(transform_, y_value_);
    canvas->DrawPoint(Point(line_->line_point(), y_pos), paint);

  } else if (state_ == kNoWave) {
  } else {
    NOTREACHED();
  }
}

void SingleLineView::OnPosChanged(MeasureLinePartView* part_view, int pos) {
  if (state_ == kInitNoWave) {
    state_ = kNoWave;
  } else if (state_ == kNoWave) {
    
  } else if (state_ == kHadWave) {
    pos_ = horiz_ 
        ? TransformReverseX(transform_, pos) 
        : TransformReverseY(transform_, pos);
  }

  UpdateLabel();
  Layout();
  SchedulePaint();
}

void SingleLineView::LayoutLabel(int pos) {
  if (pos_label_) {
    gfx::Size p_size = pos_label_->GetPreferredSize();
    gfx::Point p_point = horiz_ ? Point(pos + 2, 2) : Point(2, pos + 2);
    pos_label_->SetBoundsRect(Rect(p_point, p_size));
  }

  if (value_label_ && has_value_) {
    int y_pos = TransformY(transform_, y_value_);
    gfx::Size v_size = value_label_->GetPreferredSize();
    gfx::Point v_point(pos + 2, y_pos);
    value_label_->SetBoundsRect(Rect(v_point, v_size));
  } 

}

void SingleLineView::UpdateLabel() {
  if (pos_label_) {
    double pos_value = horiz_ 
        ? TransformReverseX(transform_, line_->line_point()) 
        : TransformReverseY(transform_, line_->line_point());
    pos_label_->SetText(StringPrintf(L"%.4f", pos_value));
  }

  if (value_label_) {
    double value;
    int x_point = line_->line_point();
    bool ret = container_view()->ValueForPoint(wave_, 
        TransformReverseX(transform_, x_point), &value);
    if (!ret) {
      value_label_->SetVisible(false);
      has_value_ = false;
      return;
    }
    value_label_->SetVisible(true);
    y_value_ = value;
    has_value_ = true;
    value_label_->SetText(StringPrintf(L"%.4f", y_value_));
  }
}

void SingleLineView::WaveChanged(Wave* wave, const gfx::Transform& transform) {
  if (state_ == kInitNoWave) {
    if (wave == NULL) {
      return;
    }
    state_ = kNoWave;
    WaveChanged(wave, transform);
    return;
  }
  else if (state_ == kNoWave) {
    if (wave == NULL) {
      return;
    }
    wave_ = wave;
    transform_ = transform;
    // use current line_pos update pos_ so measure line keep pos in view
    pos_ = horiz_ ? TransformReverseX(transform, line_->line_point()) 
        : TransformReverseY(transform, line_->line_point());
    pos_label_ = new Label();
    // label will auto calculate color need set background color
    pos_label_->SetBackgroundColor(kWaveViewBackgroundColor);
    pos_label_->SetEnabledColor(kMeasureLineColor);
    AddChildView(pos_label_);
    if (horiz_) {
      value_label_ = new Label();
      value_label_->SetBackgroundColor(kWaveViewBackgroundColor);
      value_label_->SetEnabledColor(kMeasureLineColor);
      AddChildView(value_label_);
    }
    state_ = kHadWave;

  } else if (state_ == kHadWave) {
    // stay in the current view pos
    if (wave == NULL) {
      wave_ = NULL;
      state_ = kNoWave;
      RemoveChildView(pos_label_);
      delete pos_label_;
      pos_label_ = NULL;
      RemoveChildView(value_label_);
      delete value_label_;
      value_label_ = NULL;
    } else {
      wave_ = wave;
      transform_ = transform;
      // use current line_pos update pos_ so measure line keep pos in view
      pos_ = horiz_ 
          ? TransformReverseX(transform, line_->line_point()) 
          : TransformReverseY(transform, line_->line_point());
    }

  } else {
    NOTREACHED();
  }
  UpdateLabel();
  Layout();
}

void SingleLineView::TransformChanged(const gfx::Transform& transform) {
  DCHECK(state_ == kHadWave);
  transform_ = transform;
  UpdateLabel();
  Layout();
}

void SingleLineView::DataChanged() {
  UpdateLabel();
  SchedulePaint();
}

MeasureLineContainerView* SingleLineView::container_view() {
  View* view = parent();
  return static_cast<MeasureLineContainerView*>(view);
}


bool SingleLineView::HitTestRect(const gfx::Rect& rect) const {
  gfx::Rect target_rect = rect  -  line_->GetMirroredPosition().OffsetFromOrigin();
  return line_->HitTestRect(target_rect);
}


