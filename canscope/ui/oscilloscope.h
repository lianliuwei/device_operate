#pragma once

#include "wave_control/classify_wave_control.h"

#include "canscope/device/osc_device/osc_device_handle.h"
#include "canscope/chnl/canscope_chnl_container.h"
#include "canscope/chnl/canscope_chnl.h"
#include "canscope/app/canscope_chnl_calc.h"
#include "canscope/ui/chnl_wave.h"

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
  void set_show_model(ShowModel val);

  int64 current_read_id() const { return read_id_; }
  int64 read_count() const { return read_count_; }

private:
  void HardwareDiffChanged(bool hard_diff);
  void OscConfigChanged(OscRawData* raw_data);
  void UpdateControl();
  ChnlWave* ChnlMapView(::Chnl* chnl);

  void QueueUpdate();

  void NotifyChnlAt(size_t chnl_index, int changed_set);
  void NotifyAllChnl(int changed_set);

  // implement ClassifyWaveControl
  virtual YTWaveContainer* CreateYTWaveContainer();

  ShowModel show_model_;
  OscDeviceHandle* handle_;
  ChnlCalcResultQueue::Reader queue_reader_;
  scoped_refptr<ChnlCalcResultQueue> chnl_queue_;

  scoped_ptr<CANScopeChnlContainer> chnl_container_;
  
  // order is CAN-H CAN-L CAN-DIFF
  ScopedVector<ChnlWave> chnl_waves_;
  bool last_diff_;
  int last_prefs_id_;
  OscRawDataHandle raw_data_;

  int64 read_id_;
  int64 read_count_;

  DISALLOW_COPY_AND_ASSIGN(Oscilloscope);
};

} // namespace canscope