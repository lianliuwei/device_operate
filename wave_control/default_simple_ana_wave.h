#pragma once

#include "base/memory/scoped_ptr.h"

#include "wave_control/simple_ana_wave.h"

class DefaultSimpleAnaWave : public SimpleAnaWave {
public:
  // take ownership of data
  DefaultSimpleAnaWave(string16 name, SkColor color, gfx::Image icon, AnaWaveData* data);
  virtual ~DefaultSimpleAnaWave() {}

  // implement SimpleAnaWave
  virtual AnaWaveData& Data();
  virtual WaveRange vertical_range();
  virtual void set_vertical_range(const WaveRange& wave_range);
  virtual WaveRange horizontal_range();
  virtual void set_horizontal_range(const WaveRange& wave_range);
  virtual void DoRangeCommand(int command_id, WaveRange range);
  virtual void DoCommand(int command_id);

  // implement wave
  virtual string16 name() { return name_; }
  virtual SkColor color() { return color_; }
  virtual const gfx::Image& icon() { return icon_; }

  void SetData(AnaWaveData* data);
  void SetRangeFromData();

private:
  string16 name_;
  SkColor color_;
  gfx::Image icon_;

  scoped_ptr<AnaWaveData> data_;

  WaveRange v_range_;
  WaveRange h_range_;
};