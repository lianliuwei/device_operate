#include "wave_control/examples/example_view.h"
#include "wave_control/views/wave_control_view_factory.h"
#include "wave_control/wave_control.h"
#include "wave_control/examples/test_osc_wave.h"
#include "wave_control/default_simple_ana_wave.h"
#include "wave_control/classify_wave_control.h"

#define _USE_MATH_DEFINES
#include <math.h>

namespace {
class TestWaveControl : public ClassifyWaveControl {
public:
  virtual void AddWave(Wave* wave);
  virtual void RemoveWave(Wave* wave)  {
    for (size_t i = 0; i < WaveContainerCount(); ++i) {
      WaveContainer* container = GetWaveContainerAt(i);
      YTWaveContainer* yt_container = container->AsYTWaveContainer();
      if (yt_container->HasWave(wave)) {
        yt_container->RemoveWave(wave);
        return;
      }
    }
    NOTREACHED();
  }
 

  void AddSimpleWave();

  virtual YTWaveContainer* CreateYTWaveContainer();

};

class TestWaveContainer : public YTWaveContainer {
public:


};

void TestWaveControl::AddWave(Wave* wave) {
  for (size_t i = 0; i < item_count(); ++i) {
    WaveContainer* container = GetWaveContainerAt(i);
    YTWaveContainer* yt_container = container->AsYTWaveContainer();
    if (yt_container->WaveCount() < 2) {
      yt_container->AddWave(wave);
      return;
    }
  }
  YTWaveContainer* yt_container = new TestWaveContainer();
  yt_container->AddWave(wave);
  AddWaveContainer(yt_container);
}

YTWaveContainer* TestWaveControl::CreateYTWaveContainer() {
  return new TestWaveContainer;
}

DefaultSimpleAnaWave* CreateAnaWave() {
  TestAnaData* ana_data = new TestAnaData();
  ana_data->data_range_.begin = -500;
  ana_data->data_range_.end = 500;
  ana_data->data_.reserve(400);
  for (int j = 0; j < 400; j++) {
    ana_data->data_.push_back(j * 1.0 / 400 * 500 * 1e9);
  }
  DefaultSimpleAnaWave* wave = new DefaultSimpleAnaWave(
      L"ANA_1", SkColorSetRGB(255,0,255), gfx::Image(), ana_data);
  return wave;
}

}

WaveControl* CreateWaveControl() {
  TestWaveControl* wave_control = new TestWaveControl();
   std::vector<TestOscWave*> waves  = CreateWaves();
   for (size_t i = 0; i < waves.size(); ++i) {
     wave_control->AddWave(waves[i]);
   }
  wave_control->AddWave(CreateAnaWave());
  return wave_control;
}

void SetChildBoard(views::View* const parent, int depth) {
  if (depth == 0) {
    return;
  }
  for (int i = 0; i < parent->child_count(); ++i) {
      parent->child_at(i)->set_border(
        views::Border::CreateSolidBorder(1, SkColorSetRGB(255, 255, 255)));
      SetChildBoard(parent->child_at(i), depth - 1);
  }
}
using namespace views;

void ExampleView::Init() {
  DCHECK(contents_ == NULL) << "Run called more than once.";
  WaveControl* wave_control = CreateWaveControl();
  contents_ = WaveControlViewFactory::GetInstance()->Create(wave_control);
 // SetChildBoard(contents_, 3);

  views::Widget* window =
    views::Widget::CreateWindowWithBounds(this, gfx::Rect(0, 0, 850, 400));

  window->Show();
}

ExampleView* ExampleView::CreateInstance() {
  return new ExampleView();
}