#pragma once

#include "base/string16.h"
#include "base/supports_user_data.h"
#include "ui/gfx/image/image.h"
#include "third_party/skia/include/core/SkColor.h"

class WaveControl;
class WaveContainer;
class WaveVisitor;
class OscWave;
class SimpleAnaWave;
class SimpleDigitWave;

class Wave : public base::SupportsUserData {
public:
  // parent
  WaveControl* wave_control();
  WaveContainer* wave_container();

  // RTTI
  enum Type {
    kOsc,
    kSimpleAna,
    kSimpleDigit,
  };

  virtual Type type_id() const = 0;

  virtual OscWave* AsOscWave() { return NULL; }
  virtual SimpleAnaWave* AsSimpleAnaWave() { return NULL; }
  virtual SimpleDigitWave* AsSimpleDigitWave() { return NULL; }

  virtual void Accept(WaveVisitor* visitor) = 0;

  virtual string16 name() = 0;
  virtual SkColor color() = 0;
  virtual const gfx::Image& icon() = 0;

private:
  WaveControl* wave_control_;
  WaveContainer* wave_container_;
};