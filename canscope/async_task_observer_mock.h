#pragma once

#include "testing/gmock/include/gmock/gmock.h"

#include "canscope/async_task.h"

class AsyncTaskObserverMock : public AsyncTaskObserver {
public:
  AsyncTaskObserverMock() {}
  virtual ~AsyncTaskObserverMock() {}

  MOCK_METHOD2(OnAsyncTaskStart, void(AsyncTask*, base::Value*));
  MOCK_METHOD3(OnAsyncTaskFinish, void(AsyncTask*, AsyncTaskStatus, base::Value*));
  MOCK_METHOD3(OnAsyncTaskProgress, void(AsyncTask*, int, base::Value*));
};

