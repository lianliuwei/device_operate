#pragma once

#include <map>

#include "depend_calc/calc_item.h"

class CalcData {
public:
  CalcData() {}
  virtual ~CalcData() {}

  // Derive from this class and add your own data members to associate extra
  // information with this object. Alternatively, add this as a public base
  // class to any class with a virtual destructor.
  class Data {
  public:
    virtual ~Data() {}
  };

  // use const void* support more point then CalcKey
  // take ownership of data, delete it when destroy
  Data* GetData(const void* key) const;
  void SetData(const void* key, Data* data);
  void RemoveData(const void* key);

private:
  typedef std::map<const void*, Data*> DataMap;

  // Externally-defined data accessible by key.
  DataMap user_data_;
};

template <typename T>
class CalcDataItem : public CalcData::Data {
public:
  CalcDataItem(CalcKey key, CalcData* data)
      : key_(key)
      , data_(data) {}

  ~CalcDataItem() {}

  T* Get() const {
    T* item = static_cast<T*>(data_->GetData(key_));
    return item;
  }

  void Set(T* item) {
    data_->SetData(key_, item);
  }
  
  T* operator->() const {
    return Get();
  }

private:
  CalcKey key_;
  CalcData* data_;

  DISALLOW_COPY_AND_ASSIGN(CalcDataItem);
};
