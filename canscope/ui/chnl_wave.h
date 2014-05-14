#pragma once

#include "wave_control/osc_wave.h"

#include "canscope/ui/chnl_ana_data.h"

namespace canscope {

class ChnlWave : public OscWave {
public:
  ChnlWave(::Chnl* chnl, ChnlContainer* container);
  virtual ~ChnlWave();

  // implement wave
  virtual string16 name() { return name_; }
  virtual SkColor color() { return color_; }
  virtual const gfx::Image& icon() { return icon_; }

  // implement OscWave
  virtual AnaWaveData& Data() { return &(wave_data_.get()); }
  virtual void MoveToX(double pos);
  virtual void MoveToY(double pos);
  virtual void MoveTrigger(double pos);
  virtual void ZoomInX();
  virtual void ZoomOutX();
  virtual void ZoomInY();
  virtual void ZoomOutY();
  virtual bool IsSameTrigger(OscWave* osc_wave);
  virtual WaveRange trigger_offset_range();
  virtual double trigger_offset();
  virtual bool trigger_show();
  virtual bool trigger_is_relate();
  virtual OscWave* trigger_wave();
  virtual SkColor trigger_color();
  virtual bool IsSameVertical(OscWave* osc_wave);
  virtual int vertical_div();
  virtual int vertical_window_size();
  virtual WaveRange vertical_range();
  virtual WaveRange vertical_offset_range();
  virtual double vertical_offset();
  virtual SkColor vertical_color();
  virtual bool vertical_show();
  virtual bool IsSameHorizontal(OscWave* osc_wave);
  virtual int horizontal_div();
  virtual int horizontal_window_size();
  virtual WaveRange horizontal_range();
  virtual WaveRange horizontal_offset_range();
  virtual double horizontal_offset();
  virtual SkColor horizontal_color();
  virtual bool horizontal_show();

  virtual void DoCommand(int command_id);
  virtual void DoRangeCommand(int command_id, WaveRange range);


private:
  ::Chnl* chnl_;
  ChnlContainer* container_;

  string16 name_;
  SkColor color_;
  gfx::Image icon_;
  scoped_ptr<ChnlAnaData> wave_data_;

  DISALLOW_COPY_AND_ASSIGN(ChnlWave);
};

} // namespace canscope