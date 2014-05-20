#include "canscope/ui/oscilloscope.h"

#include "base/bind.h"

#include "wave_control/yt_wave_container.h"

#include "canscope/device/canscope_device_property_constants.h"

using namespace std;
using namespace base;

namespace {
bool HasString(const vector<string>& strings, const char* key) {
  return find(strings.begin(), strings.end(), key) != strings.end();
}

}

namespace canscope {

Oscilloscope::Oscilloscope(OscDeviceHandle* handle,
                           scoped_refptr<ChnlCalcResultQueue> chnl_queue)
    : handle_(handle)
    , chnl_queue_(chnl_queue)
    , queue_reader_(chnl_queue.get())
    , read_id_(0)
    , read_count_(0)
    , show_model_(kSeparate) {
  DCHECK(handle);
  queue_reader_.set_have_data_callback(Bind(&Oscilloscope::QueueUpdate, Unretained(this)));
  queue_reader_.CallbackOnReady();
}

Oscilloscope::~Oscilloscope() {
  // let control delete them
  chnl_waves_.weak_clear();
}

void Oscilloscope::QueueUpdate() {
  scoped_refptr<ChnlCalcResult> calc_result_;
  bool ret = queue_reader_.GetResult(&calc_result_, &read_id_);
  if (ret) {
    ++read_count_;
    // get next calc_result
    queue_reader_.CallbackOnReady();

    if (chnl_container_.get() == NULL) {
      chnl_container_.reset(new CANScopeChnlContainer(calc_result_, handle_));
      last_diff_ = calc_result_->hardware_diff();
      raw_data_ = calc_result_->raw_data();
      UpdateControl();
      return;

    } else {
      chnl_container_->UpdateResult(calc_result_);
      HardwareDiffChanged(calc_result_->hardware_diff());
      OscConfigChanged(calc_result_->raw_data());
      NotifyAllChnl(OscWave::kData);
    }
  } else {
    // TODO need exit handle
  }
}

void Oscilloscope::HardwareDiffChanged(bool hard_diff) {
  if (last_diff_ == hard_diff) {
    return;
  }
  last_diff_ = hard_diff;
  UpdateControl();
}

void Oscilloscope::UpdateControl() {
  for (size_t i = 0; i < chnl_waves_.size(); ++i) {
    RemoveWave(chnl_waves_[i]);
  }
  chnl_waves_.clear();
  ChnlMapViewCallbackType chnl_map_view(Bind(&Oscilloscope::ChnlMapView, Unretained(this)));
  ::Chnl* chnl = chnl_container_->GetChnlByType(CHNL_CAN_H);
  if (chnl != NULL) {
    ChnlWave* wave = new ChnlWave(chnl, chnl_container_.get(), chnl_map_view);
    chnl_waves_.push_back(wave);
  }
  chnl = chnl_container_->GetChnlByType(CHNL_CAN_L);
  if (chnl != NULL) {
    ChnlWave* wave = new ChnlWave(chnl, chnl_container_.get(), chnl_map_view);
    chnl_waves_.push_back(wave);
  }
  chnl = chnl_container_->GetChnlByType(CHNL_CAN_DIFF);
  if (chnl != NULL) {
    ChnlWave* wave = new ChnlWave(chnl, chnl_container_.get(), chnl_map_view);
    chnl_waves_.push_back(wave);
  }

  for (size_t i = 0; i < chnl_waves_.size(); ++i) {
    AddWave(chnl_waves_[i]);
  }
}

void Oscilloscope::set_show_model(ShowModel val) {
  if (val == show_model_) {
    return;
  }
  show_model_ = val;
  // reorder Wave.
  for (size_t i = 0; i < chnl_waves_.size(); ++i) {
    RemoveWave(chnl_waves_[i]);
  }
  for (size_t i = 0; i < chnl_waves_.size(); ++i) {
    AddWave(chnl_waves_[i]);
  }
}

void Oscilloscope::AddWave(Wave* wave) {
  if (show_model_ == kCombine) {
    if (item_count() > 0) {
      WaveContainer* container = GetWaveContainerAt(0);
      YTWaveContainer* yt_container = container->AsYTWaveContainer();
      yt_container->AddWave(wave);
      return;
    }

  } else {
  }

  YTWaveContainer* yt_container = new YTWaveContainer();
  yt_container->AddWave(wave);
  AddWaveContainer(yt_container);
}

void Oscilloscope::RemoveWave(Wave* wave) {
  for (size_t i = 0; i < WaveContainerCount(); ++i) {
    WaveContainer* container = GetWaveContainerAt(i);
    YTWaveContainer* yt_container = container->AsYTWaveContainer();
    if (yt_container->HasWave(wave)) {
      yt_container->RemoveWave(wave);
      if (yt_container->WaveCount() == 0) {
        this->RemoveWaveContainer(yt_container);
      }
      return;
    }
  }
  NOTREACHED();
}

YTWaveContainer* Oscilloscope::CreateYTWaveContainer() {
  return new YTWaveContainer;
}

void Oscilloscope::OscConfigChanged(OscRawData* raw_data) {
  if (raw_data_->id() == raw_data->id()) {
    raw_data_ = raw_data;
    return;
  }

  vector<string> changed_set = raw_data_->property()->prefs_.ChangedSet(
      &(raw_data->property()->prefs_));

  if (HasString(changed_set, kOscCANHVoltRange)) {
    NotifyChnlAt(0, OscWave::kVertical);
  } else if (HasString(changed_set, kOscCANHOffset)){
    NotifyChnlAt(0, OscWave::kVerticalOffset);
  }

  if (HasString(changed_set, kOscCANLVoltRange)) {
    NotifyChnlAt(1, OscWave::kVertical);
  } else if (HasString(changed_set, kOscCANLOffset)){
    NotifyChnlAt(1, OscWave::kVerticalOffset);
  }

  if (HasString(changed_set, kOscCANDIFFVoltRange)) {
    NotifyChnlAt(2, OscWave::kVertical);
  } else if (HasString(changed_set, kOscCANDIFFOffset)){
    NotifyChnlAt(2, OscWave::kVerticalOffset);
  }

  if (HasString(changed_set, kOscTimeBase)) {
    NotifyAllChnl(OscWave::kHorizontal);
  } else if (HasString(changed_set, kOscTimeOffset)) {
    NotifyAllChnl(OscWave::kHorizontalOffset);
  }

  if (HasString(changed_set, kOscTriggerSource)) {
    NotifyAllChnl(OscWave::kTrigger);
  } else if (HasString(changed_set, kOscTriggerVolt)) {
    NotifyAllChnl(OscWave::kTriggerOffset);
  }

  raw_data_ = raw_data;
}

ChnlWave* Oscilloscope::ChnlMapView(::Chnl* chnl) {
  DCHECK(chnl);
  for (size_t i = 0; i < chnl_waves_.size(); ++i) {
    if (chnl_waves_[i]->chnl_ == chnl) {
      return chnl_waves_[i];
    }
  }
  NOTREACHED();
  return NULL;
}

void Oscilloscope::NotifyChnlAt(size_t chnl_index, int changed_set) {
  if (chnl_index >= chnl_waves_.size()) {
    return;
  }
  ChnlWave* wave = chnl_waves_[chnl_index];
  wave->NotifyChanged(changed_set);
}

void Oscilloscope::NotifyAllChnl(int changed_set) {
  for (size_t i = 0; i < chnl_waves_.size(); ++i) {
   chnl_waves_[i]->NotifyChanged(changed_set);
  }
}

} // namespace canscope
