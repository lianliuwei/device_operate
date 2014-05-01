#pragma once

#include "wave_control/views/ana_wave_data_view.h"
#include "wave_control/osc_wave.h"

class OscWaveView : public AnaWaveDataView
                  , public OscWaveObserver {
public:
  OscWaveView(OscWave* osc_wave);
  virtual ~OscWaveView();

  // other view when to get Transform, need call this.
  // this calc transform every time. no like data_transform cache.
  // this is for solve the observer notify order problem when other part
  // get notify by the WaveView Transform no update yet.
  gfx::Transform GetDataTransform();

private:
  // implement OscWaveObserver
  virtual void OnOscWaveChanged(OscWave* osc_wave, int change_set);

  // override views::View
  virtual void OnBoundsChanged(const gfx::Rect& previous_bounds) OVERRIDE;

  void UpdateTransform();
  void UpdateData();

  gfx::Transform CalcTransform();

  OscWave* osc_wave_;
};

