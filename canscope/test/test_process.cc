#include "canscope/test/test_process.h"

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
