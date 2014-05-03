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

size_t YTWaveContainer::WaveAt(Wave* wave) {
  for (size_t i = 0; i < item_count(); ++i) {
    if (GetItemAt(i) == wave) {
      return i;
    }
  }
  NOTREACHED();
  return 0;
}

bool YTWaveContainer::HasWave(Wave* wave) {
  for (size_t i = 0; i < item_count(); ++i) {
    if (GetItemAt(i) == wave) {
      return true;
    }
  }
  return false;
}

Wave* YTWaveContainer::GetWaveAt(size_t index) {
  return GetItemAt(index);
}

void YTWaveContainer::AddWave(Wave* wave) {
  Add(wave); 
}

void YTWaveContainer::RemoveWave(Wave* wave) {
  size_t i = WaveAt(wave);
  RemoveAt(i);
}

size_t YTWaveContainer::WaveCount() const {
  return item_count();
}

void YTWaveContainer::AddWaveObserver(ui::ListModelObserver* observer) {
  AddObserver(observer);
}

void YTWaveContainer::RemoveWaveObserver(ui::ListModelObserver* observer) {
  RemoveObserver(observer);
}
