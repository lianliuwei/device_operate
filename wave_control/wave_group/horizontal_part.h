#pragma once

#include "wave_control/wave_range.h"

class HorizontalPart {
public:
  virtual SkColor color() = 0;
  virtual bool show() = 0;

  virtual WaveRange range() = 0;
  virtual WaveRange offset_range() = 0;
  virtual double offset() = 0;

  // return false should no call div and window_size
  // use range as window range
  virtual bool has_div() = 0;
  virtual int div() = 0;
  virtual int window_size() = 0;

protected:
  virtual ~HorizontalPart() {}
};