#pragma once

#include "wave_control/classify_wave_control.h"

namespace canscope {

class Oscilloscope : public ClassifyWaveControl {
public:
  Oscilloscope(OscDeviceHandle* handle, 
              scoped_refptr<ChnlCalcResultQueue> chnl_queue);
  virtual ~Oscilloscope();

  // implement WaveControl
  virtual void AddWave(Wave* wave);
  virtual void RemoveWave(Wave* wave);

  enum ShowModel {
    kSeparate,
    kCombine,
  };

  ShowModel show_model() const { return show_model_; }
  void set_show_model(ShowModel val) { show_model_ = val; }

  int64 current_read_id() const;

private:
  void QueueUpdate();

  // implement ClassifyWaveControl
  virtual YTWaveContainer* CreateYTWaveContainer();

  ShowModel show_model_;
  OscDeviceHandle* handle_;
  ChnlCalcResultQueue::Reader queue_reader_;
  scoped_refptr<ChnlCalcResultQueue> chnl_queue_;

  DISALLOW_COPY_AND_ASSIGN(Oscilloscope);
};

} // namespace canscope