#include "wave_control/yt_wave_container.h"

#include "wave_control/wave_container_visitor.h"
#include "wave_control/osc_wave_group/common_osc_wave_group.h"

void YTWaveContainer::Accept(WaveContainerVisitor* visitor) {
  visitor->VisitYTWaveContainer(this);
}

OscWaveGroup* YTWaveContainer::CreateOscWaveGroup() {
  return new CommonOscWaveGroup();
}

void YTWaveContainer::SelectWave(Wave* wave) {
  Move(WaveAt(wave), 0);
}

Wave* YTWaveContainer::GetSelectWave() {
  if (item_count() == 0) {
    return NULL;
  }
  return GetItemAt(0);
}
