#pragma once

#include "ui/gfx/image/image.h"

#include "wave_control/wave_range.h"

class OscWave;

class TriggerPart {
public:
  virtual SkColor color() = 0;
  virtual bool show() = 0;
  virtual string16 text() = 0;
  virtual const gfx::Image& icon() = 0;

  virtual WaveRange offset_range() = 0;
  virtual double offset() = 0;
  virtual void set_offset(double offset) = 0;
  // Relate to Y or no.
  virtual bool IsRelate() = 0;
  // trigger wave may no in this wave group
  virtual OscWave* trigger_wave() = 0;

  virtual OscWave* osc_wave() = 0;

protected:
  virtual ~TriggerPart() {}
};
