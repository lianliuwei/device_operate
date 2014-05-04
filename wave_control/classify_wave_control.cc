#include "wave_control/classify_wave_control.h"

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
