#include "canscope/chnl/canscope_chnl_container.h"

using namespace std;

namespace {
using namespace canscope;

void InitHRange(vector<double>& range) {
  range.clear();
  for (int i = 0; i < kTimeBaseSize; ++i) {
    uint32 ns_value = TimeBaseValue(static_cast<TimeBase>(i));
    range.push_back(ns_value / 1e-9);
  }
}

}

namespace canscope {

void CANScopeChnlContainer::UpdateContainer(bool hardware_diff, bool init) {
  if (init) {
    hardware_diff_ = hardware_diff;

    if (hardware_diff) {
      can_diff_->set_type(CHNL_CAN_H);
      AddChnl(can_diff_.get());
    } else {
      can_h_->set_type(CHNL_CAN_H);
      can_l_->set_type(CHNL_CAN_L);
      can_diff_->set_type(CHNL_CAN_DIFF);
      AddChnl(can_h_.get());
      AddChnl(can_l_.get());
      AddChnl(can_diff_.get());
    }
  } else {
    if (hardware_diff == hardware_diff_) {
      return;
    }
    hardware_diff_ = hardware_diff;
    if (hardware_diff) {
      can_diff_->set_type(CHNL_CAN_H);
      RemoveChnl(can_l_.get());
      RemoveChnl(can_diff_.get());
    } else {
      can_h_->set_type(CHNL_CAN_H);
      can_l_->set_type(CHNL_CAN_L);
      can_diff_->set_type(CHNL_CAN_DIFF);
      AddChnlAt(can_h_.get(), 0);
      AddChnlAt(can_l_.get(), 1);
    }
  }
}

double CANScopeChnlContainer::HOffset() const {
  return device_property()->time_offset.value() / 1e-9;
}

void CANScopeChnlContainer::SetHOffset(double offset) const {
  CHECK(handle_);
  handle_->time_offset.set_value(offset * 1e-9);
}

int CANScopeChnlContainer::HRange() const {
  return device_property()->time_base.value();
}

const std::vector<double>& CANScopeChnlContainer::HRangeOption() const {
  return h_range_;
}

void CANScopeChnlContainer::SetHRange(int i) {
  CHECK(handle_);
  handle_->time_base.set_value(static_cast<TimeBase>(i));
}

CANScopeChnlContainer::CANScopeChnlContainer(scoped_refptr<ChnlCalcResult> result, 
                                             OscDeviceHandle* handle)
    : result_(result)
    , handle_(handle) {
  InitHRange(h_range_);
  can_h_.reset(new CANScopeChnl(CAN_H, result, handle));
  can_l_.reset(new CANScopeChnl(CAN_L, result, handle));
  can_diff_.reset(new CANScopeChnl(CAN_DIFF, result, handle));
  UpdateContainer(result_->hardware_diff(), true);
}

CANScopeChnlContainer::~CANScopeChnlContainer() {
  // delete chnl in this class.
  RemoveAll();
}

void CANScopeChnlContainer::UpdateResult(scoped_refptr<ChnlCalcResult> result) {
  result_ = result;
  UpdateContainer(result_->hardware_diff(), false);
}

OscDeviceProperty* CANScopeChnlContainer::device_property() const {
  return (const_cast<CANScopeChnlContainer*>(this))->result_->property();
}

} // namespace canscope