#include "canscope/osc_chnl_calc/chnl_calc_item.h"

#include "canscope/device/osc_raw_data.h"
#include "canscope/osc_chnl_calc/chnl_calc_result.h"

using namespace canscope;

namespace {
// raw 0 - 255 up three and down three are max and min value
// else map to value in chnl_range
inline double DecodeRaw(uint8 raw, double chnl_range) {
  if(raw < 3){
    return (-chnl_range / 2);
  }
  if (raw > 252){
    return (chnl_range / 2);
  }else{
    return chnl_range*(raw-2)/250 + (-chnl_range/2);
  }
}

void CalcChnlData(OscChnlData* can_h, OscChnlData* can_l, 
                  const OscRawData* raw_data) {
  const OscDeviceProperty* property = raw_data->property();
  const uint8* data = raw_data->data();
  DCHECK(can_h->size() == can_l->size());
  DCHECK(can_h->size()*2 == raw_data->size());
  double* data_can_h = can_h->data();
  double* data_can_l = can_l->data();
  double range_can_h = VoltRange(property->range_can_h.value());
  double range_can_l = VoltRange(property->range_can_l.value());
  int size = can_h->size();
  for (int i = 0; i < size; ++i) {
    uint8 raw_can_l = data[i * 2];
    data_can_l[i] = DecodeRaw(raw_can_l, range_can_l);

    uint8 raw_can_h = data[i * 2 + 1];
    data_can_h[i] = DecodeRaw(raw_can_h, range_can_h);
  }
}

}

namespace canscope {

ChnlCalcItem::ChnlCalcItem(std::string name, CalcKey can_h_key, CalcKey can_l_key) 
    : CalcItem(name, can_h_key)
    , can_h_key_(can_h_key)
    , can_l_key_(can_l_key) {

}

bool ChnlCalcItem::Run(CalcDelegate* delegate) {
  ChnlCalcResult* result = static_cast<ChnlCalcResult*>(delegate);
  CalcDataItem<OscChnlData> can_h(can_h_key_, result);
  CalcDataItem<OscChnlData> can_l(can_l_key_, result);

  int size = result->raw_data()->chnl_size();
  can_h.Set(new OscChnlData(size));
  can_l.Set(new OscChnlData(size));
  CalcChnlData(can_h.Get(), can_l.Get(), result->raw_data());

  return true;
}

CalcItem* ChnlCalcItem::Clone() const  {
  return new ChnlCalcItem(name(), can_h_key_, can_l_key_);
}


} // namespace canscope
