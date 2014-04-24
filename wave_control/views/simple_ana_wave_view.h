#pragma once

#include "ui/gfx/transform.h"

#include "wave_control/views/ana_wave_data_view.h"
#include "wave_control/simple_ana_wave.h"

class SimpleAnaWaveView : public AnaWaveDataView
                        , public SimpleAnaWaveObserver {
public:
  SimpleAnaWaveView(SimpleAnaWave* ana_wave);
  virtual ~SimpleAnaWaveView();

  int GetYOffset();
  void MoveToY(int y_pos);

  // overridden from views::View
  virtual bool OnMousePressed(const ui::MouseEvent& event) OVERRIDE;
  virtual bool OnMouseDragged(const ui::MouseEvent& event) OVERRIDE;
  virtual void OnMouseReleased(const ui::MouseEvent& event) OVERRIDE;

private:
  // implement SimpleAnaWaveObserver
  virtual void OnSimpleAnaWaveChanged(SimpleAnaWave* ana_wave, int change_set);
  
  // override views::View
  virtual void OnBoundsChanged(const gfx::Rect& previous_bounds) OVERRIDE;

  void UpdateTransform();
  void UpdateData();

  double handle_offset_;

  SimpleAnaWave* ana_wave_;

  // state for drag
  gfx::Point start_point_;
  gfx::Transform drag_transform_;
  WaveRange drag_v_range_;
  WaveRange drag_h_range_;
};