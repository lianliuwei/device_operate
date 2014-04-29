#include "canscope/test/test_process.h"

using namespace common;
using namespace canscope::device;

namespace {
TestProcess* g_TestProcess = NULL;
}

TestProcess::TestProcess() {
  if (g_TestProcess) {
    NOTREACHED();
  }
  g_TestProcess = this;

  for (int i = 0; i < CommonThread::ID_COUNT; ++i) {
    CommonThread::SetDelegate(static_cast<CommonThread::ID>(i), this);
  }
}


TestProcess::~TestProcess() {
  for (int i = 0; i < CommonThread::ID_COUNT; ++i) {
    CommonThread::SetDelegate(static_cast<CommonThread::ID>(i), NULL);
  }
}

void TestProcess::Init(common::CommonThread::ID id) {
  threads_[id].notifier.reset(NotificationService::Create());
  threads_[id].device_error.reset(new ScopedDeviceError);
}

void TestProcess::Init() {
  CommonThreadManager::Init();
}


void TestProcess::CleanUp(common::CommonThread::ID id) {
  threads_[id].notifier.reset(NULL);
  threads_[id].device_error.reset(NULL);
}

TestProcess* GetTestProcess() {
  return g_TestProcess;
}
void DestroyTestProcess() {
  GetTestProcess()->ShutdownThreadsAndCleanUp();
  delete g_TestProcess;
  g_TestProcess = NULL;
}
