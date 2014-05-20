#include "wave_control/views/wave_control_view_factory.h"

#include "base/memory/singleton.h"

WaveControlViewFactory* WaveControlViewFactory::GetInstance() {
  return Singleton<WaveControlViewFactory>::get();
}

WaveControlView* WaveControlViewFactory::Create(WaveControl* wave_control) {
  DCHECK(wave_control);
  return CreateImpl(wave_control);
}

views::View* WaveControlViewFactory::Create(WaveContainer* wave_container, 
                                            WaveControlView* wave_control_view) {
  DCHECK(wave_container);
  switch (wave_container->type_id()) {
  case WaveContainer::kYT:
    return CreateImpl(wave_container->AsYTWaveContainer(), wave_control_view);

  default:
    NOTREACHED() << "no support WaveContainer";
    return NULL;
  }
}

views::View* WaveControlViewFactory::Create(Wave* wave, 
                                            YTWaveContainerView* wave_container_view) {
  DCHECK(wave);                                      
  switch (wave->type_id()) {
  case Wave::kOsc:
    return CreateImpl(wave->AsOscWave(), wave_container_view);
  case Wave::kSimpleAna:
    return CreateImpl(wave->AsSimpleAnaWave(), wave_container_view);

  default:
    NOTREACHED() << "no support Wave";
    return NULL;
  }
}

WaveControlView* WaveControlViewFactory::CreateImpl(WaveControl* wave_control) {
  WaveControlView* wave_control_view = new WaveControlView(wave_control);
  return wave_control_view;
}

YTWaveContainerView* WaveControlViewFactory::CreateImpl(YTWaveContainer* wave_container, 
                                                        WaveControlView* wave_control_view) {
  YTWaveContainerView*  yt_wave_container_view 
      = new YTWaveContainerView(wave_container, wave_control_view);
  if (set_wave_bar_size_) {
    yt_wave_container_view->set_wave_bar_size(wave_bar_size_);
  }
  return yt_wave_container_view;
}

OscWaveView* WaveControlViewFactory::CreateImpl(OscWave* wave, 
                                                YTWaveContainerView* yt_wave_container_view) {
  OscWaveView* osc_wave_view = new OscWaveView(wave);
  return osc_wave_view;
}

SimpleAnaWaveView* WaveControlViewFactory::CreateImpl(SimpleAnaWave* wave, 
                                                      YTWaveContainerView* yt_wave_container_view) {
  SimpleAnaWaveView* simple_ana_wave_view = new SimpleAnaWaveView(wave);
  return simple_ana_wave_view;
}

void WaveControlViewFactory::set_wave_bar_size(int wave_bar_size) {
  set_wave_bar_size_ = true;
  wave_bar_size_ = wave_bar_size;
}

WaveControlViewFactory::WaveControlViewFactory()
  : set_wave_bar_size_(false)
  , wave_bar_size_(-1) {
}
