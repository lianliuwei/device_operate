#include "canscope/chnl/canscope_chnl.h"

#include "canscope/chnl/canscope_chnl_constants.h"

using namespace std;
namespace {
using namespace canscope;

void InitVRange(vector<double>& range) {
  range.clear();
  for (int i = 0; i < kVoltRangeSize; ++i) {
    uint32 v_value = Volt(static_cast<VoltRange>(i));
    range.push_back(v_value);
  }
}

}

namespace canscope {

ChnlTypeEnum CANScopeChnl::ChnlType() const {
  return type_;
}

bool CANScopeChnl::HasChnlData() const {
  return result_->GetData(Key()) != NULL;
}

const canscope::OscChnlData& CANScopeChnl::ChnlData() const {
  DCHECK(HasChnlData());
  CalcDataItem<OscChnlData> item(Key(), result_.get());
  return *(item.Get());
}

double CANScopeChnl::VOffset() const {
  return GetVoltOffset()->value();
}

void CANScopeChnl::SetVOffset(double offset) {
  SetVoltOffset()->set_value(offset);
}

int CANScopeChnl::VRange() const {
  return GetVoltRange()->value();
}

const std::vector<double>& CANScopeChnl::VRangeOption() const {
  return v_range_;
}

void CANScopeChnl::SetVRange(int i) {
  SetVoltRange()->set_value(static_cast<VoltRange>(i));
}

bool CANScopeChnl::IsTrigger() const {
  TriggerSource source = device_property()->trigger_source.value();
  if (!IsTriggerSourceChnl(source)) {
    return false; 
  }
  return TriggerSource2Chnl(source) == chnl_;
}

void CANScopeChnl::BeTrigger() {
  CHECK(handle_);
  handle_->trigger_source.set_value(Chnl2TriggerSource(chnl_));
}

CANScopeChnl::CANScopeChnl(canscope::Chnl chnl, 
                           scoped_refptr<ChnlCalcResult> result, 
                           OscDeviceHandle* handle)
    : chnl_(chnl)
    , result_(result) 
    , handle_(handle) {
  InitVRange(v_range_);
}

VoltRangeStoreMember* CANScopeChnl::GetVoltRange() const {
  switch (chnl_) {
  case CAN_H: return &(device_property()->range_can_h);
  case CAN_L: return &(device_property()->range_can_l);
  case CAN_DIFF: return &(device_property()->range_can_diff);
  default: NOTREACHED(); return NULL;
  }
}

Property<VoltRange>* CANScopeChnl::SetVoltRange() {
  CHECK(handle_);
  switch (chnl_) {
  case CAN_H: return &(handle_->volt_range_can_h);
  case CAN_L: return &(handle_->volt_range_can_l);
  case CAN_DIFF: return &(handle_->volt_range_can_diff);
  default: NOTREACHED(); return NULL;
  }
}

::device::DoubleStoreMember* CANScopeChnl::GetVoltOffset() const {
  switch (chnl_) {
  case CAN_H: return &(device_property()->offset_can_h);
  case CAN_L: return &(device_property()->offset_can_l);
  case CAN_DIFF: return &(device_property()->offset_can_diff);
  default: NOTREACHED(); return NULL;
  }
}

Property<double>* CANScopeChnl::SetVoltOffset() {
  CHECK(handle_);
  switch (chnl_) {
  case CAN_H: return &(handle_->offset_can_h);
  case CAN_L: return &(handle_->offset_can_l);
  case CAN_DIFF: return &(handle_->offset_can_diff);
  default: NOTREACHED(); return NULL;
  }
}

void* CANScopeChnl::Key() const {
  switch (chnl_) {
  case CAN_H: return kCANHKey;
  case CAN_L: return kCANHKey;
  case CAN_DIFF: return kCANDIFFKey;
  default: NOTREACHED(); return NULL;
  }
}

OscDeviceProperty* CANScopeChnl::device_property() const {
  return (const_cast<CANScopeChnl*>(this))->result_->property();
}

} // namespace canscope