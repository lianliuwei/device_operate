#pragma once

#include "ui/views/controls/label.h"

#include "wave_control/views/measure_line/measure_line.h"
#include "wave_control/views/measure_line/measure_line_part_view.h"

class MeasureLineContainerView;

// for point measure
// horiz show measure point value.
class SingleLineView : public views::View
                     , public MeasureLine
                     , public MeasureLinePartView::Delegate {
public:
  SingleLineView(bool horiz);
  virtual ~SingleLineView() {}

  // overridden from views::View
  virtual void OnPaint(gfx::Canvas* canvas) OVERRIDE;
  virtual void Layout() OVERRIDE;
  virtual bool HitTestRect(const gfx::Rect& rect) const OVERRIDE;

  // implement MeasureLine
  virtual void WaveChanged(Wave* wave, const gfx::Transform& transform);
  virtual void TransformChanged(const gfx::Transform& transform);
  virtual void DataChanged();


private:
  // implement MeasureLinePartView::Delegate
  virtual void OnPosChanged(MeasureLinePartView* part_view, int pos);

  void UpdateLabel();
  void LayoutLabel(int pos);
  MeasureLineContainerView* container_view();

  enum State {
    kInitNoWave,
    kHadWave,
    kNoWave,
  };
  State state_;

  double pos_;

  MeasureLinePartView* line_;
  views::Label* pos_label_;
  views::Label* value_label_;
  bool has_value_;
  double y_value_;

  gfx::Transform transform_;
  Wave* wave_;

  // init wave set before kInitNoWave
  gfx::Transform init_transform_;
  Wave* init_wave_;

  bool horiz_;
};