#pragma once

class ChnlCalcItem : public CalcItem {
public:

private:
  // may be call on other thread.
  virtual bool Run(CalcDelegate* delegate) {
    ChnlCalcResult* result = static_cast<ChnlCalcResult>(delegate);
    OscDeviceProperty* property = result->raw_data()->property();

    ResultItem<OscChnlData> can_h(result, can_h_key_);
    ResultItem<OscChnlData> can_l(result, can_l_key_);
    ResultItem<OscChnlData> can_diff(result, can_l_key_);
    can_diff.Set(new OscChnlData(can_h->size()));
    bool sub_or_avg = property->diff_ctrl.value() == kSub;
    CalcCANDIFFData(can_h.Get(), can_l.Get(), can_diff.Get(), sub_or_avg);
    
    return true;
  }

  CalcKey can_h_key_;
  CalcKey can_l_key_;
  CalcKey can_diff_key_;
};

static void CalcCANDIFFData(OscChnlData* can_h, OscChnlData* can_l, 
                            OscChnlData* can_diff, bool SubOrAvg) {
  DCHECK(can_h->size() == can_l->size());
  DCHECK(can_diff->size() == can_h->size());
  int64* data_can_h = can_h->data();
  int64* data_can_l = can_l->data();
  int64* data_can_diff = can_diff->data();
  int size = can_h->size();
  for (int i = 0; i < size; ++i) {
    if (SubOrAvg) {
      data_can_diff[i] = data_can_h[i] - data_can_l[i];
    } else {
      data_can_diff[i] = (data_can_h[i] + data_can_l[i]) / 2;
    }
  }

}
