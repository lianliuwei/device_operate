// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "wave_control/drag_test/dragged_thumb_view.h"

#include "base/stl_util.h"
#include "third_party/skia/include/core/SkShader.h"
#include "ui/gfx/canvas.h"
#include "ui/views/widget/widget.h"


namespace {
 // Saves the drawing state, and restores the state when going out of scope.
class ScopedCanvas {
 public:
  explicit ScopedCanvas(gfx::Canvas* canvas) : canvas_(canvas) {
    if (canvas_)
      canvas_->Save();
  }
  ~ScopedCanvas() {
    if (canvas_)
      canvas_->Restore();
  }
  void SetCanvas(gfx::Canvas* canvas) {
    if (canvas_)
      canvas_->Restore();
    canvas_ = canvas;
    canvas_->Save();
  }

 private:
  gfx::Canvas* canvas_;

  DISALLOW_COPY_AND_ASSIGN(ScopedCanvas);
};

}
static const int kTransparentAlpha = 200;
static const int kOpaqueAlpha = 255;
static const float kScalingFactor = 0.5;

////////////////////////////////////////////////////////////////////////////////
// DraggedTabView, public:

DraggedThumbView::DraggedThumbView(const std::vector<views::View*>& renderers,
                                   const std::vector<gfx::Rect>& renderer_bounds,
                                   const gfx::Rect& paint_bounds)
    : renderers_(renderers),
      renderer_bounds_(renderer_bounds),
      show_contents_on_drag_(true),
      paint_bounds_(paint_bounds) {
  set_owned_by_client();

  container_.reset(new views::Widget);
  views::Widget::InitParams params(views::Widget::InitParams::TYPE_POPUP);
  params.transparent = true;
  params.keep_on_top = true;
  params.ownership = views::Widget::InitParams::WIDGET_OWNS_NATIVE_WIDGET;
  params.bounds = gfx::Rect(PreferredContainerSize());
  container_->Init(params);
  container_->SetContentsView(this);
  container_->SetOpacity(kTransparentAlpha);
  container_->SetBounds(gfx::Rect(params.bounds.size()));
}

DraggedThumbView::~DraggedThumbView() {
  parent()->RemoveChildView(this);
  container_->CloseNow();
}

void DraggedThumbView::MoveTo(const gfx::Point& screen_point) {
  int x;
  gfx::Point mouse_tab_offset = paint_bounds_.origin();
  if (base::i18n::IsRTL()) {
    // On RTL locales, a dragged tab (when it is not attached to a tab strip)
    // is rendered using a right-to-left orientation so we should calculate the
    // window position differently.
    gfx::Size ps = GetPreferredSize();
    x = screen_point.x() - ScaleValue(mouse_tab_offset.x() - ps.width());
  } else {
    x = screen_point.x() + ScaleValue(mouse_tab_offset.x());
  }
  int y = screen_point.y() + ScaleValue(mouse_tab_offset.y());

#if defined(OS_WIN) && !defined(USE_AURA)
  // TODO(beng): make this cross-platform
  int show_flags = container_->IsVisible() ? SWP_NOZORDER : SWP_SHOWWINDOW;
  SetWindowPos(container_->GetNativeView(), HWND_TOP, x, y, 0, 0,
               SWP_NOSIZE | SWP_NOACTIVATE | show_flags);
#else
  gfx::Rect bounds = container_->GetWindowBoundsInScreen();
  container_->SetBounds(gfx::Rect(x, y, bounds.width(), bounds.height()));
  if (!container_->IsVisible())
    container_->Show();
#endif
}

void DraggedThumbView::Update() {
  SchedulePaint();
}

///////////////////////////////////////////////////////////////////////////////
// DraggedTabView, views::View overrides:

void DraggedThumbView::OnPaint(gfx::Canvas* canvas) {
  if (show_contents_on_drag_)
    PaintDetachedView(canvas);
  else
    PaintFocusRect(canvas);
}

gfx::Size DraggedThumbView::GetPreferredSize() {
  return paint_bounds_.size();
}

////////////////////////////////////////////////////////////////////////////////
// DraggedTabView, private:

void DraggedThumbView::PaintDetachedView(gfx::Canvas* canvas) {
  gfx::Size ps = GetPreferredSize();
  // TODO(pkotwicz): DIP enable this class.
  gfx::Canvas scale_canvas(ps, ui::SCALE_FACTOR_100P, false);
  SkBitmap& bitmap_device = const_cast<SkBitmap&>(
      skia::GetTopDevice(*scale_canvas.sk_canvas())->accessBitmap(true));
  bitmap_device.eraseARGB(0, 0, 0, 0);

  
  for (size_t i = 0; i < renderers_.size(); ++i) {
    ScopedCanvas scoped(canvas);
    gfx::Rect r_bounds = renderer_bounds_[i];
    canvas->Translate(r_bounds.OffsetFromOrigin());
    renderers_[i]->Paint(&scale_canvas);
  }

  SkBitmap mipmap = scale_canvas.ExtractImageRep().sk_bitmap();
  mipmap.buildMipMap(true);

  SkShader* bitmap_shader =
      SkShader::CreateBitmapShader(mipmap, SkShader::kClamp_TileMode,
                                   SkShader::kClamp_TileMode);

  SkMatrix shader_scale;
   shader_scale.setScale(kScalingFactor, kScalingFactor);
   bitmap_shader->setLocalMatrix(shader_scale);

  SkPaint paint;
  paint.setShader(bitmap_shader);
  paint.setAntiAlias(true);
  bitmap_shader->unref();
  
  canvas->DrawRect(gfx::Rect(ps), paint);
}

void DraggedThumbView::PaintFocusRect(gfx::Canvas* canvas) {
  gfx::Size ps = GetPreferredSize();
  canvas->DrawFocusRect(
      gfx::Rect(0, 0,
                static_cast<int>(ps.width() * kScalingFactor),
                static_cast<int>(ps.height() * kScalingFactor)));
}

gfx::Size DraggedThumbView::PreferredContainerSize() {
  gfx::Size ps = GetPreferredSize();
  return gfx::Size(ScaleValue(ps.width()), ScaleValue(ps.height()));
}

int DraggedThumbView::ScaleValue(int value) {
  return static_cast<int>(value * kScalingFactor);
}
