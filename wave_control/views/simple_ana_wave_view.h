#pragma once

#include "wave_control/views/line_data_wave_view.h"
#include "wave_control/simple_ana_wave.h"

class SimpleAnaWaveView : public LineDataWaveView
                        , public SimpleAnaWaveObserver {
public:
  SimpleAnaWaveView(SimpleAnaWave* ana_wave);
  virtual ~SimpleAnaWaveView();

  int GetYOffset();
  void MoveToY(int y_pos);

private:
  // implement SimpleAnaWaveObserver
  virtual void OnSimpleAnaWaveChanged(SimpleAnaWave* ana_wave, int change_set);
  
  // override views::View
  virtual void OnBoundsChanged(const gfx::Rect& previous_bounds) OVERRIDE;

  void UpdateTransform();
  void UpdateData();

  double handle_offset_;

  SimpleAnaWave* ana_wave_;
};