// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#pragma once

#include <vector>

#include "build/build_config.h"
#include "ui/gfx/point.h"
#include "ui/gfx/rect.h"
#include "ui/gfx/size.h"
#include "ui/views/view.h"

class DraggedThumbView : public views::View {
 public:
  // Creates a new DraggedTabView using |renderers| as the Views. DraggedTabView
  // no takes ownership of the views in |renderers|.
  // renders_bounds coordinate is paint bounds.
  // paint_bounds coordinate is mouse pos.
  DraggedThumbView(const std::vector<views::View*>& renderers,
                   const std::vector<gfx::Rect>& renderer_bounds,
                   const gfx::Rect& paint_bounds);
  virtual ~DraggedThumbView();

  // Moves the DraggedTabView to the appropriate location given the mouse
  // pointer at |screen_point|.
  void MoveTo(const gfx::Point& screen_point);

  // Notifies the DraggedTabView that it should update itself.
  void Update();

 private:
  // Overridden from views::View:
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
  virtual gfx::Size GetPreferredSize() OVERRIDE;

  // Paint the view, when it's not attached to any TabStrip.
  void PaintDetachedView(gfx::Canvas* canvas);

  // Paint the view, when "Show window contents while dragging" is disabled.
  void PaintFocusRect(gfx::Canvas* canvas);

  // Returns the preferred size of the container.
  gfx::Size PreferredContainerSize();

  // Utility for scaling a size by the current scaling factor.
  int ScaleValue(int value);

  // The window that contains the DraggedTabView.
  scoped_ptr<views::Widget> container_;

  // The renderer that paints the Tab shape.
  std::vector<views::View*> renderers_;

  // Bounds of the renderers.
  std::vector<gfx::Rect> renderer_bounds_;

  // True if "Show window contents while dragging" is enabled.
  bool show_contents_on_drag_;

  gfx::Rect paint_bounds_;

  DISALLOW_COPY_AND_ASSIGN(DraggedThumbView);
};

