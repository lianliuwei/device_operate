#include "wave_control/wave_container.h"

WaveControl* WaveContainer::wave_control() {
  return wave_control_;
}

void WaveContainer::SetWave(Wave* wave, WaveContainer* container) {
  wave->wave_container_ = container;
}

WaveContainer::WaveContainer()
    : wave_control_(NULL) { 

}
