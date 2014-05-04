#include "depend_calc/calc_data.h"

using namespace base;

CalcData::Data* CalcData::GetData(const void* key) const {
  AutoLock lock(lock_);

  DataMap::const_iterator found = user_data_.find(key);
  if (found != user_data_.end())
    return found->second.get();
  return NULL;
}

void CalcData::SetData(const void* key, Data* data) {
  linked_ptr<Data> temp;
  {
  AutoLock lock(lock_);

  DataMap::iterator found = user_data_.find(key);
  if (found != user_data_.end()) {
    temp = found->second;
    DCHECK(temp.get() != data) << "set data twice";
  }
  user_data_[key] = linked_ptr<Data>(data);
  }
  temp.release();
}

void CalcData::RemoveData(const void* key) {
  linked_ptr<Data> temp;
  {
  AutoLock lock(lock_);

  DataMap::iterator found = user_data_.find(key);
  DCHECK(found != user_data_.end());
  temp = found->second;
  }
  temp.release();
}

