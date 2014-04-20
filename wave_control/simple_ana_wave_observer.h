#pragma once

class SimpleAnaWave;

class SimpleAnaWaveObserver {
public:
  virtual void OnSimpleAnaWaveChanged(SimpleAnaWave* ana_wave, int change_set) = 0;

protected:
  SimpleAnaWaveObserver() {}
  virtual ~SimpleAnaWaveObserver() {}
};