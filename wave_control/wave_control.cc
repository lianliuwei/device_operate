#include "wave_control/wave_control.h"

#include "wave_control/wave_control_visitor.h"

void WaveControl::Accept(WaveControlVisitor* visitor) {
  visitor->VisitWaveControl(this);
}

size_t WaveControl::WaveContainerAt(WaveContainer* container) {
  for (size_t i = 0; i < item_count(); ++i) {
    if (GetItemAt(i) == container) {
      return i;
    }
  }
  NOTREACHED();
  return 0;
}

bool WaveControl::HasWaveContainer(WaveContainer* container) {
  for (size_t i = 0; i < item_count(); ++i) {
    if (GetItemAt(i) == container) {
      return true;
    }
  }
  return false;
}

void WaveControl::AddWaveContainer(WaveContainer* container) {
  SetWaveControl(container, this);
  Add(container);
}

void WaveControl::RemoveWaveContainer(WaveContainer* container) {
  SetWaveControl(container, NULL);
  size_t i = WaveContainerAt(container);
  RemoveAt(i);
}

WaveContainer* WaveControl::GetWaveContainerAt(size_t index) {
  return GetItemAt(index);
}

size_t WaveControl::WaveContainerCount() const {
  return item_count();
}

void WaveControl::AddWaveContainerObserver(ui::ListModelObserver* observer) {
  AddObserver(observer);
}

void WaveControl::RemoveWaveContainerObserver(ui::ListModelObserver* observer) {
  RemoveObserver(observer);
}

void WaveControl::SetWaveControl(WaveContainer* container, WaveControl* control) {
  container->wave_control_ = control;
}
