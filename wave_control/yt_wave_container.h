#pragma once

#include "wave_control/wave_container.h"
#include "wave_control/osc_wave_group/osc_wave_group.h"

class YTWaveContainer : public WaveContainer {
public:
  YTWaveContainer() {}
  virtual ~YTWaveContainer() {}

  // implement WaveContainer
  virtual YTWaveContainer* AsYTWaveContainer() { return this; }
  virtual void Accept(WaveContainerVisitor* visitor);
  virtual Type type_id() { return WaveContainer::kYT; }

  // caller take ownership
  virtual OscWaveGroup* CreateOscWaveGroup();

  virtual void SelectWave(Wave* wave);
  virtual Wave* GetSelectWave();

  size_t WaveAt(Wave* wave);
  bool HasWave(Wave* wave);
  Wave* GetWaveAt(size_t index);
  // may layout the wave in subclass
  virtual void AddWave(Wave* wave); 
  // no delete wave
  virtual void RemoveWave(Wave* wave);
  size_t WaveCount() const;
  void AddWaveObserver(ui::ListModelObserver* observer);
  void RemoveWaveObserver(ui::ListModelObserver* observer);

private:
  DISALLOW_COPY_AND_ASSIGN(YTWaveContainer);
};

