#include "wave_control/views/line_data_wave_view.h"

#include <math.h>

#include "base/logging.h"
#include "ui/gfx/point.h"
#include "ui/gfx/canvas.h"

#include "wave_control/views/transform_util.h"

using namespace gfx;

namespace {
static const int kAutoShowDotThreshold = 10;
static const int kDotWidth = 4;
static const int kLineWidth = 1;

// return true for have intersect, false for no
bool RangeIntersect(int* intersect_left, int* intersect_right, 
                    int range_1_left, int range_1_right, 
                    int range_2_left, int range_2_right) {
    CHECK(range_1_left <= range_1_right) << "the coord is defalut windows coord";
    CHECK(range_2_left <= range_2_right) << "the coord is defalut windows coord";
    if(range_1_right < range_2_left || range_2_right < range_1_left)
      return false;
    *intersect_left = std::max(range_1_left, range_2_left);
    *intersect_right = std::min(range_1_right, range_2_right);
    return true;
}

}

bool LineDataWaveView::PaintWaveParam(int* vector_start_out, int* vector_end_out,
                                      int* plot_begin_out, int* plot_end_out,
                                      gfx::Transform* vector_to_real_x_out,
                                      bool* auto_show_dot_out,
                                      bool* need_sample_out) {

  WaveRange wave_range = line_data_->data_range();
  int real_begin = TransformX(data_transform_, wave_range.begin);
  int real_end = TransformX(data_transform_, wave_range.end);
  int real_length = real_end - real_begin;
  CHECK(real_begin <= real_end);
  int view_begin = GetLocalBounds().x();
  int view_end = GetLocalBounds().right();
  int plot_begin, plot_end;
  bool ret = RangeIntersect(&plot_begin, &plot_end, 
                             real_begin, real_end, 
                             view_begin, view_end);
  // no data need to show
  if (!ret) {
    return false;
  }

  int vector_size = static_cast<int>(line_data_->size());
  gfx::Transform vector_to_real_x;
  // convert to float manual, or the divide is using int lost precision
  vector_to_real_x.Scale(static_cast<float>(real_length) / (vector_size - 1), 1);
  vector_to_real_x.Translate(real_begin, 0);
  // get How many point to show
  int vector_start = TransformReverseX(vector_to_real_x, plot_begin);
  int vector_end = TransformReverseX(vector_to_real_x, plot_end);
  // add two point separate add begin and end for show the wave like cut when
  // the wave range is cut off by the view.
  if (vector_start != 0)
    vector_start -= 1;
  if (vector_end != vector_size -1)
    vector_end += 1;

  bool auto_show_dot = vector_size == 1 ? true : 
    real_length / (vector_size - 1) >= kAutoShowDotThreshold;
  bool need_sample = 2 * real_length < vector_size;

  if (vector_start_out)
    *vector_start_out = vector_start;
  if (vector_end_out)
    *vector_end_out = vector_end;
  if (plot_begin_out)
    *plot_begin_out = plot_begin;
  if (plot_end_out)
    *plot_end_out = plot_end;
  if (vector_to_real_x_out)
    *vector_to_real_x_out = vector_to_real_x;
  if (auto_show_dot_out)
    *auto_show_dot_out = auto_show_dot;
  if (need_sample_out)
    *need_sample_out = need_sample;

  return true;
}

void LineDataWaveView::PaintWave(gfx::Canvas* canvas) {
  if (line_data_ || line_data_->data() == NULL)
    return;
 
  int vector_start, vector_end, plot_begin, plot_end;
  gfx::Transform vector_to_real_x;
  bool auto_show_dot, need_sample;

  bool ret = PaintWaveParam(&vector_start, &vector_end, 
                            &plot_begin, &plot_end, 
                            &vector_to_real_x, 
                            &auto_show_dot, &need_sample);
  if (!ret) {
    return;
  }

  // show line, show dot
  bool draw_line = show_sytle_ != kDot;
  bool draw_dot = show_sytle_ == kDot || show_sytle_ == kLineAndDot ||
      (show_sytle_ == KLineAndAutoDot) && auto_show_dot;

  // prepare the paint
  SkPaint dot_paint;
  dot_paint.setStrokeWidth(kDotWidth);
  dot_paint.setColor(dot_color_);
  SkPaint line_paint;
  line_paint.setStrokeWidth(kLineWidth);
  line_paint.setColor(wave_color_);
  line_paint.setAntiAlias(true);

  AnaWaveData* buffer = line_data_;
  double* data = line_data_->data();
  // need sample for range.
  if (need_sample) {
    // pixel by pixel
    double begin_value = data[vector_start];
    int begin_y = TransformY(data_transform_, begin_value);

    for (int i = plot_begin; i < plot_end; i ++) {
      int begin_index = TransformReverseX(vector_to_real_x, i);
      int end_index = TransformReverseX(vector_to_real_x, i + 1);
      PeakValue peak = buffer->GetRangePeak(begin_index, end_index - begin_index);
      int begin = TransformY(data_transform_, peak.begin);
      int end = TransformY(data_transform_, peak.end);
      int max = TransformY(data_transform_, peak.min);
      int min = TransformY(data_transform_, peak.max);
      
      if (draw_line) {
        canvas->DrawLine(Point(i, begin_y), Point(i + 1, begin), line_paint);
        canvas->DrawLine(Point(i, max), Point(i, min), line_paint);
      }
      if (draw_dot) {
        canvas->DrawPoint(Point(i, begin), dot_paint);
        canvas->DrawPoint(Point(i, end), dot_paint);
        canvas->DrawPoint(Point(i, max), dot_paint);
        canvas->DrawPoint(Point(i, min), dot_paint);
      }
      begin_y = end;
    }

  // draw all the point.
  } else {
    // TODO test if drawPoints can save plot time
    // draw a line segment and the back dot
    for (int i = vector_start; i < vector_end; ++i) {
      int begin_x = TransformX(vector_to_real_x, i);
      int begin_y = TransformY(data_transform_, data[i]);
      int end_x = TransformX(vector_to_real_x, i+1);
      int end_y = TransformY(data_transform_, data[i+1]);
      if (draw_line) {
        canvas->DrawLine(Point(begin_x, begin_y), Point(end_x, end_y), line_paint);
      }
      if (draw_dot) {
        canvas->DrawPoint(Point(begin_x, begin_y), dot_paint);
      }
    }
  }
  // draw the last dot;
  if (draw_dot) {
    double logic_y = data[vector_end];
    Point point(TransformX(vector_to_real_x, vector_end), 
                TransformY(data_transform_, logic_y));
    canvas->DrawPoint(point, dot_paint);
  }
}

void LineDataWaveView::OnPaint(gfx::Canvas* canvas) {
  views::View::OnPaint(canvas); // draw the background border first.
  PaintWave(canvas);
}

bool LineDataWaveView::HasHitTestMask() const  {
  return false;
}

bool LineDataWaveView::OnMouseDragged(const ui::MouseEvent& event) {
  return false;
}

void LineDataWaveView::set_wave_color(SkColor color) {
  if (color != wave_color_) {
    wave_color_ = color;
    SchedulePaint();    
  }
}

void LineDataWaveView::set_dot_color(SkColor color) {
  if (color != dot_color_) {
    dot_color_ = color;
    SchedulePaint();    
  }
}

void LineDataWaveView::set_show_style(ShowStyle style) {
  if (style != show_sytle_) {
    show_sytle_ = style;
    SchedulePaint();    
  }
}

void LineDataWaveView::set_line_data(AnaWaveData* line_data) {
  // data allow no the same, just repaint it.
  line_data_ = line_data;
  SchedulePaint();
}

void LineDataWaveView::set_data_transform(const gfx::Transform& data_transform) {
  if (data_transform != data_transform_) {
    data_transform_ = data_transform;
    SchedulePaint();
  }
}
