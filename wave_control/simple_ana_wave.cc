#include "wave_control/simple_ana_wave.h"

#include "wave_control/wave_visitor.h"

void SimpleAnaWave::Accept(WaveVisitor* visitor) {
  visitor->VisitSimpleAnaWave(this);
}

void SimpleAnaWave::AddObserver(SimpleAnaWaveObserver* observer) {
  observer_list_.AddObserver(observer);
}

void SimpleAnaWave::RemoveObserver(SimpleAnaWaveObserver* observer) {
  observer_list_.RemoveObserver(observer);
}

void SimpleAnaWave::HasObserver(SimpleAnaWaveObserver* observer) {
  observer_list_.HasObserver(observer);
}

void SimpleAnaWave::NotifyChanged(int change_set) {
  FOR_EACH_OBSERVER(SimpleAnaWaveObserver, observer_list_, OnSimpleAnaWaveChanged(this, change_set));
}
