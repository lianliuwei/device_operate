#pragma once

#include "ui/gfx/transform.h"
#include "wave_control/wave.h"

class MeasureLine {
public:
  virtual ~MeasureLine() {}

  virtual void WaveChanged(Wave* wave, const gfx::Transform& transform) = 0;
  virtual void TransformChanged(const gfx::Transform& transform) = 0;
  virtual void DataChanged() = 0;
};