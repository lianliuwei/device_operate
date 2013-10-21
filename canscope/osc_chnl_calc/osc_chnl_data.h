#pragma once

// chnl data with vertical offset 
class OscChnlData : public CalcData::Data {
public:
  OscChnlData(int size);
  virtual ~OscChnlData();

  int size() const;
  int64* data();
  const int64* data() const;

};
