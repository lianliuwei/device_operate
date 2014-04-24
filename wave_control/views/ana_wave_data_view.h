#pragma once

#include "ui/gfx/transform.h"
#include "ui/views/view.h"

#include "wave_control/ana_wave_data.h"

// LineDataWaveView plot the wave Form of the Line Data, it sample or cut the
// wave manual to achieve high freq plot (this is key in Osc app) no matter the
// data range is large or the data is many. 
// the data is own by the controller.
// input data are logic value using the Logic to real transform Matrix 
// to Transform
class AnaWaveDataView : public views::View {
public:
  enum ShowStyle {
    kLine,
    kDot, 
    kLineAndDot,
    // when two point away from each other then threshold value show the dot.
    KLineAndAutoDot,
  };

  AnaWaveDataView() {};
  virtual ~AnaWaveDataView() {};

  void set_wave_color(SkColor color);
  SkColor wave_color() const;

  void set_dot_color(SkColor color);
  SkColor dot_color() const;

  void set_show_style(ShowStyle style);
  ShowStyle show_style() const;

  // set the data for show, the data is own by others.
  void set_line_data(AnaWaveData* line_data);
  AnaWaveData* line_data() const;

  void set_data_transform(const gfx::Transform& data_transform);
  gfx::Transform data_transform() { return data_transform_; }

private:
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
  
  // for test hit on the wave, so we can move the wave
  virtual bool HasHitTestMask() const OVERRIDE;

  // for dragged the wave
  virtual bool OnMouseDragged(const ui::MouseEvent& event) OVERRIDE;

  void PaintWave(gfx::Canvas* canvas);

  bool PaintWaveParam(int* vector_start_out, int* vector_end_out,
                      int* plot_begin_out, int* plot_end_out,
                      gfx::Transform* vector_to_real_x_out,
                      bool* auto_show_dot_out,
                      bool* need_sample_out);
private:
  // data to plot on the View
  AnaWaveData* line_data_;

  SkColor wave_color_;
  SkColor dot_color_;
  ShowStyle show_sytle_;

  gfx::Transform data_transform_;
};

