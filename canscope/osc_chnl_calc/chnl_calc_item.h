#pragma once

class OscChnlData;

template <typename T>
class ResultItem : public base::SupportsUserData::Data {
  T* Get() {
    T* item = static_cast<T*>(data_->GetData(key_));
    return item;
  }

  void Set(T* item) {

  }
private:
  CalcKey key_;
  CalcData* data_;
};

class ChnlCalcItem : public CalcItem {
public:

private:
  // may be call on other thread.
  virtual bool Run(CalcDelegate* delegate) {
    ChnlCalcResult* result = static_cast<ChnlCalcResult>(delegate);
    ResultItem<OscChnlData> can_h(result, can_h_key_);
    ResultItem<OscChnlData> can_l(result, can_l_key_);
    
    CalcChnlData(can_h.Get(), can_l.Get(), result->raw_data());
    
    return true;
  }

  CalcKey can_h_key_;
  CalcKey can_l_key_;
};

// 
static void CalcChnlData(OscChnlData* can_h, OscChnlData* can_l, 
                         const OscRawData* raw_data) {
  OscDeviceProperty* property = raw_data->property();
  const uint8* data = raw_data->data();
  DCHECK(can_h->size(), can_l->size());
  DCHECK(can_h->size()*2, raw_data->size());
  int64* data_can_h = can_h->data();
  int64* data_can_l = can_l->data();
  int64 range_can_h = VoltRangeValue(property->range_can_h.value());
  int64 range_can_l = VoltRangeValue(property->range_can_l.value());;
  int size = can_h->size();
  for (int i = 0; i < size; ++i) {
    uint8 raw_can_l = data[i * 2];
    data_can_l[i] = DecodeRaw(raw_can_l, range_can_l);

    uint8 raw_can_h = data[i * 2 + 1];
    data_can_h[i] = DecodeRaw(raw_can_h, range_can_h);
  }

}

// raw 0 - 255 up three and down three are max and min value
// else map to value in chnl_range
inline int64 DecodeRaw(uint8 raw, int64 chnl_range) {
  if(raw < 3){
    return (-chnl_range /2);
  }
  if (raw > 252){
    return (chnl_range / 2);
  }else{
    return chnl_range*(raw-2)/250.0 + (-chnl_range/2.0);
  }
}
int64 VoltRangeValue(VoltRange range) {

}
class MathChnlCalcItem : public CalcItem {
public:
  MathChnlCalcItem(CalcKey first, CalcKey second);


  virtual bool Run(CalcDelegate* delegate) {

  }
};
