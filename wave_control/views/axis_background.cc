#include "wave_control/views/axis_background.h"

#include "ui/gfx/canvas.h"
#include "ui/views/view.h"

// TODO() maybe use double buffer. when size no change no redraw.
void AxisBackground::Paint(gfx::Canvas* canvas, views::View* view) const {
  // Fill the background. Note that we don't constrain to the bounds as
  // canvas is already clipped for us.
  canvas->DrawColor(get_color());
  // paint the axis on the view but no the border area that paint by the border
  // itself.
  gfx::Rect rect = view->GetContentsBounds();
  views::Painter::PaintPainterAt(canvas, axis_painter_.get(), rect);
}

bool AxisBackground::NormalSize( gfx::Size& size ) {
    return axis_painter_->NormalSize(size);
}

gfx::Size AxisBackground::GetMinimumSize() {
    return axis_painter_->GetMinimumSize();
}

AxisBackground::AxisBackground(SkColor background, 
                               SkColor line, int line_width, 
                               SkColor grid, 
                               int v_grid, int v_grid_div, 
                               int h_grid, int h_grid_div) 
    : axis_painter_(new AxisPainter(line, line_width, 
                                    grid, 
                                    v_grid, v_grid_div, 
                                    h_grid, h_grid_div)) {
    SetNativeControlColor(background);
}