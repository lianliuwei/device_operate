#pragma once

#include "ui/base/models/list_model.h"

#include "wave_control/wave.h"

class WaveControl;
class YTWaveContainer;
class YLWaveContainer;
class XYWaveContainer;
class WaveContainerVisitor;

class WaveContainer : protected ui::ListModel<Wave> {
public:
  WaveContainer();
  virtual ~WaveContainer() {}

  // parent
  WaveControl* wave_control();

  enum Type {
    kYT, // Y , time
    kYL, // Y , log X
    kXY,
  };

  virtual Type type_id() = 0;

  virtual YTWaveContainer* AsYTWaveContainer() { return NULL; }
  virtual YLWaveContainer* AsYLWaveContainer() { return NULL; }
  virtual XYWaveContainer* AsXYWaveContainer() { return NULL; }

  virtual void Accept(WaveContainerVisitor* visitor) = 0;

protected:
  void SetWave(Wave* wave, WaveContainer* container);

private:
  friend WaveControl;

  WaveControl* wave_control_;

  DISALLOW_COPY_AND_ASSIGN(WaveContainer);
};
