#pragma once

#include <map>

#include "base/memory/ref_counted.h"
#include "base/memory/linked_ptr.h"
#include "base/synchronization/lock.h"

#include "depend_calc/calc_item.h"

// take ownership of the Data set to CalcData.
// thread safe Get()  and Set().
class CalcData : public base::RefCountedThreadSafe<CalcData> {
public:
  CalcData() {}

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

  // check key no set before
  void SetData(const void* key, Data* data);
  void RemoveData(const void* key);

protected:
  friend class base::RefCountedThreadSafe<CalcData>;
  virtual ~CalcData() {}

private:
  typedef std::map<const void*, linked_ptr<Data> > DataMap;

  // Externally-defined data accessible by key.
  DataMap user_data_;
  
  mutable base::Lock lock_;
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
};
