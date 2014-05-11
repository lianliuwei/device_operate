#include "wave_control/classify_wave_control.h"
#include "wave_control/yt_wave_container.h"

namespace {
static const int kWaveContainerOrder[] = {
  0, // kYT
  1, // KYL
  2, // KXY
};

}

void ClassifyWaveControl::AddWaveContainer(WaveContainer* container) {
  size_t insert_index = WaveContainerCount();
  for (size_t i = 0; i < WaveContainerCount(); ++i) {
    WaveContainer::Type type = GetWaveContainerAt(i)->type_id();
    int current_order = kWaveContainerOrder[type];
    int order = kWaveContainerOrder[container->type_id()];
    if (current_order < order) {
      continue;
    } else if (current_order > order) {
      insert_index = i;
      break;
    }
  }
  SetWaveControl(container, this);
  AddAt(insert_index, container);
}

bool ClassifyWaveControl::CANMoveWaveTo(Wave* wave, WaveContainer* container) {
  if (wave->wave_control() != this) {
    return false;
  }
  // already in WaveContainer
  if (wave->wave_container() == container) {
    return false;
  }
  // only support YTWaveContainer wave move
  return container->AsYTWaveContainer() != NULL 
      && wave->wave_container()->AsYTWaveContainer() != NULL;
}

bool ClassifyWaveControl::CANCreateWaveContainerAt(Wave* wave, size_t index) {
  if (wave->wave_control() != this) {
    return false;
  }
  // no YT wave can no move
  if (wave->wave_container()->AsYTWaveContainer() == NULL) {
    return false;
  }
  DCHECK(index >= 0 && index <= WaveContainerCount());
  // if check after last one, check last one
  if (index == WaveContainerCount()) {
    --index;
  }
  WaveContainer* container = GetWaveContainerAt(index);

  return container->AsYTWaveContainer() != NULL;
}

void ClassifyWaveControl::MoveWaveTo(Wave* wave, WaveContainer* container) {
  DCHECK(CANMoveWaveTo(wave, container));
  YTWaveContainer* old_container = wave->wave_container()->AsYTWaveContainer();
  old_container->RemoveWave(wave);
  container->AsYTWaveContainer()->AddMovedWave(wave);

  if (old_container->WaveCount() == 0) {
    RemoveWaveContainer(old_container);
    delete old_container;
  }
}

void ClassifyWaveControl::CreateWaveContainerAt(Wave* wave, size_t index) {
  DCHECK(CANCreateWaveContainerAt(wave, index));
  YTWaveContainer* old_container = wave->wave_container()->AsYTWaveContainer();
  old_container->RemoveWave(wave);
  YTWaveContainer* new_container = CreateYTWaveContainer();
  new_container->AddMovedWave(wave);
  SetWaveControl(new_container, this);
  AddAt(index, new_container);

  if (old_container->WaveCount() == 0) {
    RemoveWaveContainer(old_container);
    delete old_container;
  }
}
