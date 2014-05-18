#include "canscope/ui/chnl_wave.h"

#include "canscope/chnl/canscope_chnl_container.h"

using namespace canscope;

namespace {
const wchar_t* ChnlName(ChnlTypeEnum chnl) {
  switch (chnl) {
  case CHNL_CAN_H: return L"CAN-H";
  case CHNL_CAN_L: return L"CAN-L";
  case CHNL_CAN_DIFF: return L"CAN-DIFF";
  case CHNL_CAN_USE: return L"CAN-USE";
  default: NOTREACHED(); return L"";
  }
}

SkColor ChnlColor(ChnlTypeEnum chnl) {
  switch (chnl) {
  case CHNL_CAN_H: return SkColorSetRGB(255, 255, 0);
  case CHNL_CAN_L: return SkColorSetRGB(0, 128, 0);
  case CHNL_CAN_DIFF: return SkColorSetRGB(255, 0, 0);
  case CHNL_CAN_USE: return SkColorSetRGB(255, 0, 255);
  default: NOTREACHED(); return SkColorSetRGB(0, 0, 0);;
  }
}

const int kVerticalDiv = 8;
const int kHorizontalDiv = 10;
const SkColor kHorizonColor = SkColorSetRGB(240, 240, 240);
}

namespace canscope {

ChnlWave::ChnlWave(::Chnl* chnl, ChnlContainer* container, ChnlMapViewCallbackType chnl_map_view)
    : chnl_(chnl)
    , container_(container)
    , chnl_map_view_(chnl_map_view) {
  DCHECK(chnl);
  name_ = ChnlName(chnl->ChnlType());
  color_ = ChnlColor(chnl->ChnlType());
}

void ChnlWave::MoveToX(double pos) {
  container_->SetHOffset(pos);
}

void ChnlWave::MoveToY(double pos) {
  chnl_->SetVOffset(pos);
}

void ChnlWave::MoveTrigger(double pos) {
  chnl_->SetTriggerOffset(pos);
}

void ChnlWave::ZoomInX() {
 int select = container_->HRange();
 --select;
 if (select < 0) {
   return;
 }
 container_->SetHRange(select);
}

void ChnlWave::ZoomOutX() {
  int select = container_->HRange();
  ++select;
  if (static_cast<size_t>(select) >= container_->HRangeOption().size()) {
    return;
  }
  container_->SetHRange(select);
}

void ChnlWave::ZoomInY() {
  int select = chnl_->VRange();
  --select;
  if (select < 0) {
    return;
  }
  chnl_->SetVRange(select);
}

void ChnlWave::ZoomOutY() {
  int select = chnl_->VRange();
  ++select;
  if (static_cast<size_t>(select) >= chnl_->VRangeOption().size()) {
    return;
  }
  chnl_->SetVRange(select);
}

bool ChnlWave::IsSameTrigger(OscWave* osc_wave) {
  // NOTE now all OscWave is ChnlWave
  ChnlWave* wave = static_cast<ChnlWave*>(osc_wave);
  return container_ == wave->container_;
}

WaveRange ChnlWave::trigger_offset_range() {
  DCHECK(chnl_->IsTrigger());
  return vertical_offset_range();
}

double ChnlWave::trigger_offset() {
 return chnl_->TriggerOffset();
}

bool ChnlWave::trigger_show() {
  return chnl_->IsTrigger();
}

bool ChnlWave::trigger_is_relate() {
  return true;
}

OscWave* ChnlWave::trigger_wave() {
  if (chnl_map_view_.is_null()) {
    return NULL;
  }
  for (int i = 0; i < container_->ChnlCount(); ++i) {
    ::Chnl* chnl = container_->GetChnlAt(i);
    if (chnl_->IsTrigger()) {
      return chnl_map_view_.Run(chnl);
    }
  }
  return NULL;
}

SkColor ChnlWave::trigger_color() {
  OscWave* osc_wave = trigger_wave();
  if (osc_wave == NULL) {
    return SkColorSetRGB('b', 'a', 'd');
  }
  return osc_wave->color();
}

bool ChnlWave::IsSameVertical(OscWave* osc_wave) {
  return false;
}

int ChnlWave::vertical_div() {
  return kVerticalDiv;
}

int ChnlWave::vertical_window_size() {
  return vertical_div();
}

WaveRange ChnlWave::vertical_range() {
 WaveRange range;
 range.begin = chnl_->VRangeValue() / 2;
 range.end = - chnl_->VRangeValue() / 2;
 return range;
}

WaveRange ChnlWave::vertical_offset_range() {
  WaveRange range = vertical_range();
  range.begin *= 2;
  range.end *= 2;
  return range;
}

double ChnlWave::vertical_offset() {
  return chnl_->VOffset();
}

SkColor ChnlWave::vertical_color() {
  return color_;
}

bool ChnlWave::vertical_show() {
  return true;
}

bool ChnlWave::IsSameHorizontal(OscWave* osc_wave) {
  ChnlWave* wave = static_cast<ChnlWave*>(osc_wave);
  return container_ == wave->container_;
}

int ChnlWave::horizontal_div() {
  return kHorizontalDiv;
}

// TODO depend on device
int ChnlWave::horizontal_window_size() {
  return horizontal_div();
}

WaveRange ChnlWave::horizontal_range() {
  WaveRange range;
  range.begin = container_->HRangeValue() / 2;
  range.end = - container_->HRangeValue() / 2;
  return range;
}

WaveRange ChnlWave::horizontal_offset_range() {
  WaveRange range = horizontal_range();
  range.begin *= 2;
  range.end *= 2;
  return range;
}

double ChnlWave::horizontal_offset() {
  return container_->HOffset();
}

SkColor ChnlWave::horizontal_color() {
  return kHorizonColor;
}

bool ChnlWave::horizontal_show() {
  return true;
}

void ChnlWave::DoCommand(int command_id) {
  NOTIMPLEMENTED();
}

void ChnlWave::DoRangeCommand(int command_id, WaveRange range) {
  NOTIMPLEMENTED();
}


} // namespace canscope