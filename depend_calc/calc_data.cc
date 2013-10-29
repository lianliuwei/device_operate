#include "depend_calc/calc_data.h"

CalcData::Data* CalcData::GetData(const void* key) const {
  DataMap::const_iterator found = user_data_.find(key);
  if (found != user_data_.end())
    return found->second;
  return NULL;
}

void CalcData::SetData(const void* key, Data* data) {
  DataMap::iterator found = user_data_.find(key);
  if (found != user_data_.end())
    delete found->second;
  user_data_[key] = data;
}

void CalcData::RemoveData(const void* key) {
  DataMap::iterator found = user_data_.find(key);
  if (found != user_data_.end())
    delete found->second;
}

