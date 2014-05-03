#pragma once

#include "base/observer_list.h"

#include "wave_control/wave.h"
#include "wave_control/ana_wave_data.h"
#include "wave_control/simple_ana_wave_observer.h"

class SimpleAnaWave : public Wave {
public:
  SimpleAnaWave() {}
  virtual ~SimpleAnaWave() {}

  // implement wave
  virtual SimpleAnaWave* AsSimpleAnaWave() OVERRIDE { return this; }
  virtual Type type_id() const OVERRIDE { return kSimpleAna; }
  virtual void Accept(WaveVisitor* visitor) OVERRIDE;

  virtual AnaWaveData& Data() = 0;

  // just for save the range, so no notify to the view.
  // all change is done be the view.
  virtual WaveRange vertical_range() = 0;
  virtual void set_vertical_range(const WaveRange& wave_range) = 0;

  virtual WaveRange horizontal_range() = 0;
  virtual void set_horizontal_range(const WaveRange& wave_range) = 0;

  // command
  // for do FFT.
  virtual void DoRangeCommand(int command_id, WaveRange range) = 0;
 
  virtual void DoCommand(int command_id) = 0;

  enum UpdateType {
    kVertical = 1 << 0,
    kHorizontal = 1 << 2,
    kData = 1<< 2,
  };

  void AddObserver(SimpleAnaWaveObserver* observer);
  void RemoveObserver(SimpleAnaWaveObserver* observer);
  void HasObserver(SimpleAnaWaveObserver* observer);

protected:
  void NotifyChanged(int change_set);

private:
  ObserverList<SimpleAnaWaveObserver> observer_list_;

  DISALLOW_COPY_AND_ASSIGN(SimpleAnaWave);
};
