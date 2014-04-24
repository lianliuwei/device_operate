#pragma once

#include "wave_control/views/ana_wave_data_view.h"
#include "wave_control/osc_wave.h"

class OscWaveView : public AnaWaveDataView
                  , public OscWaveObserver {
public:
  OscWaveView(OscWave* osc_wave);
  virtual ~OscWaveView();

private:
  // implement OscWaveObserver
  virtual void OnOscWaveChanged(OscWave* osc_wave, int change_set);

  // override views::View
  virtual void OnBoundsChanged(const gfx::Rect& previous_bounds) OVERRIDE;

  void UpdateTransform();
  void UpdateData();

  OscWave* osc_wave_;
};

