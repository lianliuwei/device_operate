#pragma once

#include "ui/base/models/list_model.h"

#include "wave_control/wave_container.h"

class WaveControlVisitor;

// no sub class now
class WaveControl : protected ui::ListModel<WaveContainer> {
public:
  WaveControl() {}
  virtual ~WaveControl() {}

  size_t WaveContainerAt(WaveContainer* container);
  bool HasWaveContainer(WaveContainer* container);
  WaveContainer* GetWaveContainerAt(size_t index);
  // can sort WaveContainer in subclass
  virtual void AddWaveContainer(WaveContainer* container);
  // no delete container
  virtual void RemoveWaveContainer(WaveContainer* container);
  size_t WaveContainerCount() const;
  void AddWaveContainerObserver(ui::ListModelObserver* observer);
  void RemoveWaveContainerObserver(ui::ListModelObserver* observer);

  // auto layout wave
  virtual void AddWave(Wave* wave) = 0;
  virtual void RemoveWave(Wave* wave) = 0;

  virtual void Accept(WaveControlVisitor* visitor);

  // drag move wave
  virtual bool CANMoveWaveTo(Wave* wave, WaveContainer* container) = 0;
  virtual bool CANCreateWaveContainerAt(Wave* wave, size_t index) = 0;
  virtual void MoveWaveTo(Wave* wave, WaveContainer* container) = 0;
  virtual void CreateWaveContainerAt(Wave* wave, size_t index) = 0;

protected:
  void SetWaveControl(WaveContainer* container, WaveControl* control);

private:
  DISALLOW_COPY_AND_ASSIGN(WaveControl);
};
