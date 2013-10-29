#pragma once

#include "common/common_thread_manager.h"

class TestProcess : public common::CommonThreadManager {
public:
  TestProcess();
  virtual ~TestProcess() {}

private:
};

TestProcess* GetTestProcess();
void DestroyTestProcess();
