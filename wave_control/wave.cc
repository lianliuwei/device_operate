#include "wave_control/wave.h"

#include "wave_control/wave_container.h"

WaveControl* Wave::wave_control() {
  if (wave_container() == NULL) {
    return NULL;
  }
  return wave_container()->wave_control();
}

WaveContainer* Wave::wave_container() {
  return wave_container_;
}

Wave::Wave()
    : wave_container_(NULL) {}


