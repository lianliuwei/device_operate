#include "canscope/ui/chnl_wave.h"

using namespace canscope;

namespace {
const wchar_t* ChnlName(ChnlTypeEnum chnl) {
  switch (chnl) {
  case CHNL_CAN_H: return L"CAN-H";
  case CHNL_CAN_L: return L"CAN-L";
  case CHNL_CAN_DIFF: return L"CAN-DIFF";
  case CHNL_CAN_USE: return L"CAN-USE";
  default: NOTREACHED() return L"";
  }
}

SkColor ChnlColor(ChnlTypeEnum chnl) {
  switch (chnl) {
  case CHNL_CAN_H: return SkColorSetRGB(255, 255, 0);
  case CHNL_CAN_L: return SkColorSetRGB(0, 128, 0);
  case CHNL_CAN_DIFF: return SkColorSetRGB(255, 0, 0);
  case CHNL_CAN_USE: return SkColorSetRGB(255, 0, 255);
  default: NOTREACHED() return SkColorSetRGB(0, 0, 0);;
  }
}

}

namespace canscope {

ChnlWave::ChnlWave(::Chnl* chnl, ChnlContainer* container)
    : chnl_(chnl)
    , container_(container) {
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
  if (select >= container_->HRangeOption().size()) {
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
  if (select >= chnl_->VRangeOption().size()) {
    return;
  }
  chnl_->SetVRange(select);
}

bool ChnlWave::IsSameTrigger(OscWave* osc_wave) {
  // now all OscWave is ChnlWave
  ChnlWave* wave = static_cast<ChnlWave*>(osc_wave);
  return container_ == wave->container_;
}

WaveRange ChnlWave::trigger_offset_range() {
  DCHECK(chnl_->BeTrigger());
  return vertical_offset_range();
}

double ChnlWave::trigger_offset() {
 return chnl_->TriggerOffset();
}

bool ChnlWave::trigger_show() {
  return chnl_->BeTrigger();
}

bool ChnlWave::trigger_is_relate() {
  return true;
}

OscWave* ChnlWave::trigger_wave() {

}



} // namespace canscope