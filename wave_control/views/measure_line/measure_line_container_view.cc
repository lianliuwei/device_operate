#include "wave_control/views/measure_line/measure_line_container_view.h"

#include "wave_control/views/all_fill_layout.h"
#include "wave_control/osc_wave_observer.h"
#include "wave_control/simple_ana_wave_observer.h"
#include "wave_control/osc_wave.h"
#include "wave_control/simple_ana_wave.h"
#include "wave_control/wave_visitor.h"

using namespace std;

namespace {
bool IsSet(int set, int test) {
  return (set & test) != 0;
}

class WaveObserver : public OscWaveObserver
                   , public SimpleAnaWaveObserver {
public:
  WaveObserver(MeasureLineContainerView* view);
  virtual ~WaveObserver();

  void SetWave(Wave* wave);

private:
  // implement OscWaveObserver
  virtual void OnOscWaveChanged(OscWave* osc_wave, int change_set);
  // implement SimpleAnaWaveObserver
  virtual void OnSimpleAnaWaveChanged(SimpleAnaWave* ana_wave, int change_set);

  Wave* wave_;
  MeasureLineContainerView* view_;

  DISALLOW_COPY_AND_ASSIGN(WaveObserver);
};

WaveObserver::WaveObserver(MeasureLineContainerView* view) 
    : view_(view)
    , wave_(NULL) {
}


WaveObserver::~WaveObserver() {
  SetWave(NULL);
}

void WaveObserver::OnOscWaveChanged(OscWave* osc_wave, int change_set) {
  int update_transform = OscWave::kVertical | OscWave::kVerticalOffset 
    | OscWave::kHorizontal | OscWave::kHorizontalOffset;
  if (IsSet(change_set, update_transform)) {
    view_->TransformChanged();
  } 
  if (IsSet(change_set, OscWave::kData)) {
    view_->DataChanged();
  }
}

void WaveObserver::OnSimpleAnaWaveChanged(SimpleAnaWave* ana_wave, int change_set) {
  int update_transform = SimpleAnaWave::kVertical | SimpleAnaWave::kVertical;
  if (IsSet(change_set, update_transform)) {
    view_->TransformChanged();
  }
  if (IsSet(change_set, SimpleAnaWave::kData)) {
    view_->DataChanged();
  }
}

class YTWaveVisitor : public WaveVisitor {
public:
  YTWaveVisitor(WaveObserver* observer);

  void AddWave(Wave* wave);
  void RemoveWave(Wave* wave);
  AnaWaveData* GetAnaWaveData(Wave* wave);

private:
  enum VisitorType {
    kAddWave,
    kRemoveWave,
    kGetAnaData,
  };

  // implement WaveVisitor
  virtual void VisitOscWave(OscWave* wave);
  virtual void VisitSimpleAnaWave(SimpleAnaWave* wave);
  virtual void VisitSimpleDigitWave(SimpleDigitWave* wave) { NOTREACHED(); }

  VisitorType type_;
  WaveObserver* observer_;
  AnaWaveData* wave_data_;

  DISALLOW_COPY_AND_ASSIGN(YTWaveVisitor);
};


YTWaveVisitor::YTWaveVisitor(WaveObserver* observer) 
  : observer_(observer) {
}

void YTWaveVisitor::AddWave(Wave* wave) {
  type_ = kAddWave;
  wave->Accept(this);
}

void YTWaveVisitor::RemoveWave(Wave* wave) {
  type_ = kRemoveWave;
  wave->Accept(this);
}

AnaWaveData* YTWaveVisitor::GetAnaWaveData(Wave* wave) {
  type_ = kGetAnaData;
  wave_data_ = NULL;
  wave->Accept(this);
  return wave_data_;
}

void YTWaveVisitor::VisitOscWave(OscWave* wave) {
  if (type_ == kAddWave) {
    wave->AddObserver(observer_);
  } else if (type_ == kRemoveWave) {
    wave->RemoveObserver(observer_);
  } else if (type_ == kGetAnaData) {
    wave_data_ =  &(wave->Data());
  } else {
    NOTREACHED();
  }
}

void YTWaveVisitor::VisitSimpleAnaWave(SimpleAnaWave* wave) {
  if (type_ == kAddWave) {
    wave->AddObserver(observer_);
  } else if (type_ == kRemoveWave) {
    wave->RemoveObserver(observer_);
  } else if (type_ == kGetAnaData) {
    wave_data_ = &(wave->Data());
  } else {
    NOTREACHED();
  }
}


void WaveObserver::SetWave(Wave* wave) {
  YTWaveVisitor visitor(this);
  if (wave_ != NULL) {
   visitor.RemoveWave(wave_);
  }
  if (wave != NULL) {
    visitor.AddWave(wave);
  }
  wave_ = wave;
}

}

MeasureLineContainerView::MeasureLineContainerView(Delegate* delegate)
    : delegate_(delegate)
    , horiz_single_(NULL)
    , vertical_single_(NULL)
    , inited_(false) {
  SetLayoutManager(new AllFillLayout);
  wave_observer_.reset(new WaveObserver(this));
}


MeasureLineContainerView::~MeasureLineContainerView() {

}

void MeasureLineContainerView::ShowHorizSingle(bool show) {
  if (show == IsShowHorizSingle()) {
    return;
  }
  if (show) {
    horiz_single_ = new SingleLineView(true);
    AddChildView(horiz_single_);
    InitMeasureLine(horiz_single_);
  } else {
    RemoveChildView(horiz_single_);
    delete horiz_single_;
    horiz_single_ = NULL;
    SchedulePaint();
  }
}

bool MeasureLineContainerView::IsShowHorizSingle() {
  return horiz_single_ != NULL;
}

void MeasureLineContainerView::ToggleShowHorizSingle() {
  ShowHorizSingle(!IsShowHorizSingle());
}


void MeasureLineContainerView::ShowVerticalSingle(bool show) {
  if (show == IsShowVerticalSingle()) {
    return;
  }
  if (show) {
    vertical_single_ = new SingleLineView(false);
    AddChildView(vertical_single_);
    InitMeasureLine(vertical_single_);
  } else {
    RemoveChildView(vertical_single_);
    delete vertical_single_;
    vertical_single_ = NULL;
    SchedulePaint();
  }
}

bool MeasureLineContainerView::IsShowVerticalSingle() {
  return vertical_single_ != NULL;
}

void MeasureLineContainerView::ToggleShowVerticalSingle() {
  return ShowVerticalSingle(!IsShowVerticalSingle());
}

void MeasureLineContainerView::InitMeasureLine(MeasureLine* measure_line) {
  if (!inited_) {
    return;
  }
  measure_line->WaveChanged(delegate_->GetMeasureWave(), 
      delegate_->GetMeasureWaveTransform());
}

void MeasureLineContainerView::MeasureWaveChanged() {
  wave_observer_->SetWave(delegate_->GetMeasureWave());

  vector<MeasureLine*> lines = measure_lines();
  for (size_t i = 0; i < lines.size(); ++i) {
    (lines[i])->WaveChanged(delegate_->GetMeasureWave(), delegate_->GetMeasureWaveTransform());
  }
}

void MeasureLineContainerView::TransformChanged() {
  if (!inited_) {
    return;
  }
  vector<MeasureLine*> lines = measure_lines();
  for (size_t i = 0; i < lines.size(); ++i) {
    (lines[i])->TransformChanged(delegate_->GetMeasureWaveTransform());
  }
}

void MeasureLineContainerView::DataChanged() {
  if (!inited_) {
    return;
  }
  vector<MeasureLine*> lines = measure_lines();
  for (size_t i = 0; i < lines.size(); ++i) {
    (lines[i])->DataChanged();
  }
}

std::vector<MeasureLine*> MeasureLineContainerView::measure_lines() {
  vector<MeasureLine*> lines;
  if (horiz_single_) {
    lines.push_back(horiz_single_);
  }
  if (vertical_single_) {
    lines.push_back(vertical_single_);
  }
  return lines;
}

bool MeasureLineContainerView::ValueForPoint(Wave* wave, double x_pos, double* value) {
  YTWaveVisitor visitor(wave_observer_.get());
  AnaWaveData* wave_data = visitor.GetAnaWaveData(wave);
  if (wave_data == NULL) {
    return false;
  }
  WaveRange data_range = wave_data->data_range();
  if (!data_range.InRange(x_pos)) {
    return false;
  }
  double y_value = wave_data->GetValue(x_pos);
  if (value) {
    *value = y_value;
  }
  return true;
}

bool MeasureLineContainerView::HitTestRect(const gfx::Rect& rect) const  {
  bool ret;
  if (horiz_single_) {
    ret = horiz_single_->HitTestRect(rect);
    if (ret) {
      return true;
    }
  }
  if (vertical_single_) {
    ret = vertical_single_->HitTestRect(rect);
    if (ret) {
      return true;
    }
  }
  return false;
}

void MeasureLineContainerView::InitMeasureLine() {
  DCHECK(!inited_);
  vector<MeasureLine*> lines = measure_lines();
  for (size_t i = 0; i < lines.size(); ++i) {
    (lines[i])->WaveChanged(delegate_->GetMeasureWave(), 
        delegate_->GetMeasureWaveTransform());
  }
  inited_ = true;
}

