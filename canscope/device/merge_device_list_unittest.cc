#include "testing/gtest/include/gtest/gtest.h"

#include "canscope/device/canscope_device_finder.h"
#include "canscope/device/devices_manager_observer.h"

#include "base/vlog.h"

using namespace std;
using namespace canscope;

typedef std::map<string16, DeviceStatus> DeviceStatusMap;

void MergeDeviceList(bool& list_change, bool& state_change,
                     DeviceStatusMap& device_to_status_, vector<string16> devices)
{
  // using set operate
  vector<string16> off_list;
  for (DeviceStatusMap::iterator it = device_to_status_.begin(); 
      it != device_to_status_.end(); ++it) {
    vector<string16>::iterator finded = 
      find(devices.begin(), devices.end(), it->first);
    // already exist
    if (finded != devices.end()) {
      devices.erase(finded);
      if (it->second == kOffline) {
        it->second = kOnline;
        state_change = true;
      }
    } else {
      off_list.push_back(it->first);
    }
  }
  for (std::vector<string16>::iterator it = off_list.begin();
      it != off_list.end(); ++it) {
    if (device_to_status_[*it] == kExist) {
      list_change = true;
      device_to_status_.erase(*it);
    } else if (device_to_status_[*it] == kOnline) {
      state_change = true;
      device_to_status_[*it] = kOffline;
      LOG(INFO) << "enum device find device offline";
    }
  }
  for (std::vector<string16>::iterator it = devices.begin(); 
      it != devices.end(); ++it) {
    list_change = true;
    device_to_status_.insert(std::make_pair(*it, kExist));
  }
}

TEST(MergeDeviceListTest, MergeList) {
  bool list_change = false;
  bool state_change = false;

  DeviceStatusMap device_to_status;
  device_to_status[string16(L"1")] = kExist;
  device_to_status[string16(L"2")] = kExist;
  device_to_status[string16(L"3")] = kExist;

  vector<string16> devices;
  devices.push_back(L"2");
  devices.push_back(L"3");
  devices.push_back(L"4");

  MergeDeviceList(list_change, state_change, device_to_status, devices);

  EXPECT_EQ(true, list_change);
  EXPECT_EQ(false, state_change);
  EXPECT_EQ(3, device_to_status.size());

  EXPECT_EQ(kExist, device_to_status[L"2"]);
  EXPECT_EQ(kExist, device_to_status[L"3"]);
  EXPECT_EQ(kExist, device_to_status[L"4"]);
}

TEST(MergeDeviceListTest, Add) {
  bool list_change = false;
  bool state_change = false;

  DeviceStatusMap device_to_status;
  device_to_status[string16(L"2")] = kExist;
  device_to_status[string16(L"3")] = kOffline;
  device_to_status[string16(L"4")] = kOnline;

  vector<string16> devices;
  devices.push_back(L"1");
  devices.push_back(L"2");
  devices.push_back(L"3");
  devices.push_back(L"4");
  devices.push_back(L"5");

  MergeDeviceList(list_change, state_change, device_to_status, devices);

  EXPECT_EQ(true, list_change);
  EXPECT_EQ(true, state_change);
  EXPECT_EQ(5, device_to_status.size());

  EXPECT_EQ(kExist, device_to_status[L"1"]);
  EXPECT_EQ(kExist, device_to_status[L"2"]);
  EXPECT_EQ(kOnline, device_to_status[L"3"]);
  EXPECT_EQ(kOnline, device_to_status[L"4"]);
  EXPECT_EQ(kExist, device_to_status[L"5"]);
}

TEST(MergeDeviceListTest, Remove) {
  bool list_change = false;
  bool state_change = false;

  DeviceStatusMap device_to_status;
  device_to_status[string16(L"1")] = kExist;
  device_to_status[string16(L"2")] = kExist;
  device_to_status[string16(L"3")] = kOffline;
  device_to_status[string16(L"4")] = kOnline;

  vector<string16> devices;
  devices.push_back(L"2");
  devices.push_back(L"3");

  MergeDeviceList(list_change, state_change, device_to_status, devices);

  EXPECT_EQ(true, list_change);
  EXPECT_EQ(true, state_change);
  EXPECT_EQ(3, device_to_status.size());

  EXPECT_EQ(kExist, device_to_status[L"2"]);
  EXPECT_EQ(kOnline, device_to_status[L"3"]);
  EXPECT_EQ(kOffline, device_to_status[L"4"]);
}

TEST(MergeDeviceListTest, ChangeStatus) {
  bool list_change = false;
  bool state_change = false;

  DeviceStatusMap device_to_status;
  device_to_status[string16(L"2")] = kExist;
  device_to_status[string16(L"3")] = kOffline;
  device_to_status[string16(L"4")] = kOnline;

  vector<string16> devices;
  devices.push_back(L"2");
  devices.push_back(L"3");
  devices.push_back(L"4");

  MergeDeviceList(list_change, state_change, device_to_status, devices);

  EXPECT_EQ(false, list_change);
  EXPECT_EQ(true, state_change);
  EXPECT_EQ(3, device_to_status.size());

  EXPECT_EQ(kExist, device_to_status[L"2"]);
  EXPECT_EQ(kOnline, device_to_status[L"3"]);
  EXPECT_EQ(kOnline, device_to_status[L"4"]);
}