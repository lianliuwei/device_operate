#include "canscope/osc_chnl_calc/can_diff_calc_item.h"

#include "base/logging.h"

#include "canscope/osc_chnl_calc/chnl_calc_result.h"

using namespace canscope;

namespace {
static void CalcCANDIFFData(OscChnlData* can_h, OscChnlData* can_l, 
                            OscChnlData* can_diff, bool SubOrAvg) {
  DCHECK(can_h->size() == can_l->size());
  DCHECK(can_diff->size() == can_h->size());
  double* data_can_h = can_h->data();
  double* data_can_l = can_l->data();
  double* data_can_diff = can_diff->data();
  int size = can_h->size();
  for (int i = 0; i < size; ++i) {
    if (SubOrAvg) {
      data_can_diff[i] = data_can_h[i] - data_can_l[i];
    } else {
      data_can_diff[i] = (data_can_h[i] + data_can_l[i]) / 2;
    }
  }

}

}

ChnlDiffCalcItem::ChnlDiffCalcItem(std::string name, 
                                   CalcKey can_h_key, 
                                   CalcKey can_l_key, 
                                   CalcKey can_diff_key)
    : CalcItem(name, can_diff_key)
    , can_h_key_(can_h_key)
    , can_l_key_(can_l_key)
    , can_diff_key_(can_diff_key) {

}

bool ChnlDiffCalcItem::Run(CalcDelegate* delegate) {
  ChnlCalcResult* result = static_cast<ChnlCalcResult*>(delegate);
  OscDeviceProperty* property = result->raw_data()->property();

  CalcDataItem<OscChnlData> can_h(can_h_key_, result);
  CalcDataItem<OscChnlData> can_l(can_l_key_, result);
  CalcDataItem<OscChnlData> can_diff(can_diff_key_, result);

  DCHECK(can_h.Get() != NULL);
  DCHECK(can_l.Get() != NULL);
  can_diff.Set(new OscChnlData(can_h->size()));
  bool sub_or_avg = property->diff_ctrl.value() == kSub;
  CalcCANDIFFData(can_h.Get(), can_l.Get(), can_diff.Get(), sub_or_avg);

  return true;
}

CalcItem* ChnlDiffCalcItem::Clone() const  {
  return new ChnlDiffCalcItem(name(), can_h_key_, can_l_key_, can_diff_key_);
}
