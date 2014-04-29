#pragma once

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "common/common_thread.h"
#include "common/common_thread_impl.h"

namespace base {
class MessageLoop;
}

namespace common {
class CommonThreadImpl;

class COMMON_EXPORT CommonThreadManager {
 public:
  typedef CommonThreadImpl MainThreadType;
  typedef CommonThreadImpl ThreadType;

  CommonThreadManager();
  virtual ~CommonThreadManager();

  // call InitializeMainThread() and CreateThreads()
  void Init();

  void InitializeMainThread();

  // Create all secondary threads.
  void CreateThreads();

  void MainMessageLoopRun();
 
  // Performs the shutdown sequence, starting with PostMainMessageLoopRun
  // through stopping threads to PostDestroyThreads.
  void ShutdownThreadsAndCleanUp();

  int GetResultCode() const { return result_code_; }

 protected:

  // The main message loop has been started at this point (but has not
  // been run), and the toolkit has been initialized. Returns the error code
  // (or 0 if no error).
  virtual int PreCreateThreads() {
    return 0;
  }

  // This is called just before the main message loop is run.  The
  // various browser threads have all been created at this point
  virtual void PreMainMessageLoopRun() {}

  // This happens after the main message loop has stopped, but before
  // threads are stopped.
  virtual void PostMainMessageLoopRun() {}

  // Called as the very last part of shutdown, after threads have been
  // stopped and destroyed.
  virtual void PostDestroyThreads() {}

  // Members initialized on construction ---------------------------------------
  int result_code_;

  // Members initialized in |MainMessageLoopStart()| ---------------------------
  scoped_ptr<base::MessageLoop> main_message_loop_;

  // Members initialized in |InitializeMainThread()| ---------------------------
  // This must get destroyed before other threads that are created in parts_.
  scoped_ptr<MainThreadType> main_thread_;

  scoped_ptr<ThreadType> threads_[CommonThread::ID_COUNT];


  DISALLOW_COPY_AND_ASSIGN(CommonThreadManager);
  
};
}; // namespace common
