#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "base/threading/thread.h"
#include "base/synchronization/waitable_event.h"

#include "canscope/async_task.h"
#include "canscope/async_task_observer_mock.h"

using testing::_;

namespace {
class AsyncTaskTest;

class TestThread : public base::Thread {
public:
  TestThread(AsyncTaskTest* test)
      : Thread("AsyncTask")
      , test_(test) {
    DCHECK(test);
  }

  virtual ~TestThread() {}

private:
  virtual void Init();
  virtual void CleanUp();

  AsyncTaskTest* test_;
};

class AsyncTaskTest : public testing::Test {
public:
  AsyncTaskTest()
      : thread_(this)
      , event(false, false) {}

  void Signal() {
    event.Signal();
  }

  void Verify() {
    testing::Mock::VerifyAndClearExpectations(&observer);
  }

protected:
  virtual void SetUp() {
    thread_.Start();
  }
  virtual void TearDown() {
    thread_.Stop();
  }

  friend class TestThread;

  AsyncTaskObserverMock observer;
  AsyncTaskHandle task;
  base::WaitableEvent event;

private:
  TestThread thread_;
};


void TestThread::Init() {
  test_->task = new AsyncTask;
  test_->task->AddObserver(&(test_->observer));
  test_->event.Signal();
}

void TestThread::CleanUp() {
  test_->task->RemoveObserver(&(test_->observer));
}

}

ACTION_P(SignalEvent, test) {
  test->Signal();
}
TEST_F(AsyncTaskTest, NormalUse) {
  event.Wait();
  ASSERT_TRUE(task.get());
  EXPECT_TRUE(task->IsReady());

  // Start
  EXPECT_CALL(observer, OnAsyncTaskStart(task.get(), NULL))
      .Times(1).WillOnce(SignalEvent(this));
  EXPECT_CALL(observer, OnAsyncTaskFinish(_, _, _)).Times(0);
  EXPECT_CALL(observer, OnAsyncTaskProgress(_, _, _)).Times(0);

  task->NotifyStart(NULL);
  event.Wait();
  Verify();  
  EXPECT_TRUE(task->IsRunning());

  // Progress
  EXPECT_CALL(observer, OnAsyncTaskStart(_, _)).Times(0);
  EXPECT_CALL(observer, OnAsyncTaskFinish(_, _, _)).Times(0);
  EXPECT_CALL(observer, OnAsyncTaskProgress(task.get(), 10, NULL))
      .Times(1).WillOnce(SignalEvent(this));

  task->NotifyProgress(true, 10, NULL);
  event.Wait();
  Verify();
  EXPECT_TRUE(task->IsRunning());

  // Progress
  EXPECT_CALL(observer, OnAsyncTaskStart(_, _)).Times(0);
  EXPECT_CALL(observer, OnAsyncTaskFinish(_, _, _)).Times(0);
  EXPECT_CALL(observer, OnAsyncTaskProgress(task.get(), 20, NULL))
      .Times(1).WillOnce(SignalEvent(this));

  task->NotifyProgress(true, 10, NULL);
  event.Wait();
  Verify();
  EXPECT_EQ(20, task->GetProgress());
  EXPECT_TRUE(task->IsRunning());

    // Progress
  EXPECT_CALL(observer, OnAsyncTaskStart(_, _)).Times(0);
  EXPECT_CALL(observer, OnAsyncTaskFinish(_, _, _)).Times(0);
  EXPECT_CALL(observer, OnAsyncTaskProgress(task.get(), 30, NULL))
      .Times(1).WillOnce(SignalEvent(this));

  task->NotifyProgress(false, 30, NULL);
  event.Wait();
  Verify();
  EXPECT_EQ(30, task->GetProgress());
  EXPECT_TRUE(task->IsRunning());

  // Finish
  EXPECT_CALL(observer, OnAsyncTaskStart(_, _)).Times(0);
  EXPECT_CALL(observer, OnAsyncTaskFinish(task.get(), kFinish, NULL))
      .Times(1).WillOnce(SignalEvent(this));
  EXPECT_CALL(observer, OnAsyncTaskProgress(_, _, _)).Times(0);

  task->NotifyFinish(NULL);
  event.Wait();
  Verify();
  EXPECT_TRUE(task->IsFinish());
}
