#pragma once

#include "ui/views/view.h"

#include "wave_control/views/measure_line/measure_line.h"
#include "wave_control/views/measure_line/single_line_view.h"

namespace {
class WaveObserver;
}

class MeasureLineContainerView : public views::View {
public:
  class Delegate {
  public:
    virtual ~Delegate() {}
    virtual Wave* GetMeasureWave() = 0;
    virtual const gfx::Transform GetMeasureWaveTransform() = 0;

  };
  MeasureLineContainerView(Delegate* delegate);
  virtual ~MeasureLineContainerView();

  // get Value for Point
  bool ValueForPoint(Wave* wave, double x_pos, double* value);

  // horiz or vertical mean to the object for measure
  void ShowHorizSingle(bool show);
  bool IsShowHorizSingle();
  void ToggleShowHorizSingle();

  void ShowHorizDual(bool show);
  bool IsShowHorizDual();
  void ToggleShowHorizDual();

  void ShowVerticalSingle(bool show);
  bool IsShowVerticalSingle();
  void ToggleShowVerticalSingle();

  void ShowVerticalDual(bool show);
  bool IsShowVerticalDual();
  void ToggleShowVerticalDual();

  void MeasureWaveChanged();
  void TransformChanged();
  void DataChanged();

private:
  // override views::View
  // size changed means the transform is changed. so just pull it.
  virtual void OnBoundsChanged(const gfx::Rect& previous_bounds) OVERRIDE;

  void NotifyMeasureLineWaveChanged(MeasureLine* measure_line);
  
  std::vector<MeasureLine*> measure_lines();

  SingleLineView* horiz_single_;
  SingleLineView* vertical_single_;
  
  Delegate* delegate_;

  scoped_ptr<WaveObserver> wave_observer_;

  DISALLOW_COPY_AND_ASSIGN(MeasureLineContainerView);
};