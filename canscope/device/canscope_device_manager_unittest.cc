#include "base/message_loop.h"
#include "base/synchronization/waitable_event.h"

#include "common/common_thread_manager.h"
#include "common/common_thread.h"

#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "canscope/device/canscope_device_manager.h"
#include "canscope/device/canscope_device_manager_handle.h"
#include "canscope/canscope_notification_types.h"

using namespace base;
using namespace common;
using namespace canscope;

class TestProcess : public common::CommonThreadManager {
public:
  TestProcess();
  virtual ~TestProcess() {}

private:
};
namespace {
TestProcess* g_TestProcess = NULL;
}


TestProcess::TestProcess() {
  if (g_TestProcess) {
    NOTREACHED();
  }
  g_TestProcess = this;
}

TestProcess* GetTestProcess() {
  return g_TestProcess;
}
void DestroyTestProcess() {
  GetTestProcess()->ShutdownThreadsAndCleanUp();
  delete g_TestProcess;
  g_TestProcess = NULL;
}

class CANScopeDeviceManagerTest : public testing::Test
                                , public NotificationObserver {
public:
  CANScopeDeviceManagerTest()
    : event_(true, false) {}
  virtual ~CANScopeDeviceManagerTest() {}

  static void SetUpTestCase() {
    new TestProcess();
    GetTestProcess()->Init();
  }
  static void TearDownTestCase() {
    DestroyTestProcess();
  }

private:
  virtual void SetUp() {
    CommonThread::PostTask(CommonThread::DEVICE, FROM_HERE, 
        Bind(&CANScopeDeviceManagerTest::CreateDeviceManager, Unretained(this)));
    event_.Wait();
    CANScopeDeviceManagerHandle::Create();
    registrar_.Add(this, NOTIFICATION_DEVICE_MANAGER_DESTROYED, 
        Source<DeviceManager>(CANScopeDeviceManager::GetInstance()));
  }

  virtual void TearDown() {
    registrar_.RemoveAll();
  }

  void CreateDeviceManager() {
    CANScopeDeviceManager::Create();
    event_.Signal();
  }

  virtual void Observe(int type, 
                       const NotificationSource& source, 
                       const NotificationDetails& details) {
    if (type == NOTIFICATION_DEVICE_MANAGER_DESTROYED) {
      MessageLoop::current()->QuitWhenIdle();
    }
  }


  WaitableEvent event_;
  NotificationRegistrar registrar_;
};

TEST_F(CANScopeDeviceManagerTest, GetValue) {

  CommonThread::PostTask(CommonThread::DEVICE, FROM_HERE, 
      Bind(&DeviceManager::StartDestroying, CANScopeDeviceManager::GetInstance()));
  GetTestProcess()->MainMessageLoopRun();
}
