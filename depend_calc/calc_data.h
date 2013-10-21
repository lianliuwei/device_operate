#pragma once

class CalcData {
public:
  CalcData();
  virtual ~CalcData();

  // Derive from this class and add your own data members to associate extra
  // information with this object. Alternatively, add this as a public base
  // class to any class with a virtual destructor.
  class BASE_EXPORT Data {
  public:
    virtual ~Data() {}
  };

  // take ownership of data, delete it when destroy
  Data* GetData(const void* key) const;
  void SetData(const void* key, Data* data);
  void RemoveData(const void* key);

private:
  typedef std::map<const void*, Data*> DataMap;

  // Externally-defined data accessible by key.
  DataMap user_data_;
};
