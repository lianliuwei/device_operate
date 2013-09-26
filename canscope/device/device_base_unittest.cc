#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "base/threading/thread.h"
#include "base/bind.h"

#include "canscope/device/device_base.h"
#include "canscope/device/scoped_device_lock.h"

using namespace base;

class DeviceMock: public DeviceBase {
public:
  DeviceMock() 
      : DeviceBase(NULL) {}
  ~DeviceMock() {}

  MOCK_METHOD0(LockStatusChanged, void());

private:
  virtual canscope::ValueMapDevicePropertyStore* DevicePrefs() {
    NOTIMPLEMENTED();
    return NULL;
  }

  DISALLOW_COPY_AND_ASSIGN(DeviceMock);
};

TEST(DeviceBaseTest, Lock) {
  DeviceMock device;
  EXPECT_CALL(device, LockStatusChanged()).Times(2);
  {
  ScopedDeviceLock lock(&device, "test", false);
  EXPECT_TRUE(lock.IsLocked());
  EXPECT_TRUE(kNullSeq != lock.seq());
  EXPECT_TRUE(device.IsLocked());
  EXPECT_TRUE(device.CheckLock(lock.seq()));
  }

  EXPECT_CALL(device, LockStatusChanged()).Times(2);
  {
  ScopedDeviceLock lock(&device, "test", true);
  EXPECT_TRUE(lock.IsLocked());
  EXPECT_TRUE(kNullSeq != lock.seq());
  EXPECT_TRUE(device.IsLocked());
  EXPECT_TRUE(device.CheckLock(lock.seq()));
  }
}

void DeviceLock(DeviceMock* device) {
  ScopedDeviceLock lock(device, "test", true);
  EXPECT_TRUE(lock.IsLocked());
  EXPECT_TRUE(kNullSeq != lock.seq());
  EXPECT_TRUE(device->IsLocked());
  EXPECT_TRUE(device->CheckLock(lock.seq()));

}

TEST(DeviceBaseTest, ThreadLock) {
  Thread thread1("thread1"), thread2("thread2"), thread3("thread3");
  thread1.Start();
  thread2.Start();
  thread3.Start();

  DeviceMock device;
  EXPECT_CALL(device, LockStatusChanged()).Times(12);

  thread1.message_loop_proxy()->PostTask(FROM_HERE, Bind(&DeviceLock, &device));
  thread1.message_loop_proxy()->PostTask(FROM_HERE, Bind(&DeviceLock, &device));
  thread2.message_loop_proxy()->PostTask(FROM_HERE, Bind(&DeviceLock, &device));
  thread2.message_loop_proxy()->PostTask(FROM_HERE, Bind(&DeviceLock, &device));
  thread3.message_loop_proxy()->PostTask(FROM_HERE, Bind(&DeviceLock, &device));
  thread3.message_loop_proxy()->PostTask(FROM_HERE, Bind(&DeviceLock, &device));

  thread1.Stop();
  thread2.Stop();
  thread3.Stop();

}
