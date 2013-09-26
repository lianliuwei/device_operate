#include "common/common_thread_manager.h"

#include "base/bind.h"
#include "base/command_line.h"
#include "base/debug/trace_event.h"
#include "base/logging.h"
#include "base/message_loop.h"
#include "base/metrics/histogram.h"
#include "base/pending_task.h"
#include "base/run_loop.h"
#include "base/process/process_metrics.h"
#include "base/process_util.h"
#include "common/common_thread_impl.h"

namespace common {
CommonThreadManager::CommonThreadManager()
    : result_code_(0) {
}

CommonThreadManager::~CommonThreadManager() {

}

void CommonThreadManager::Init() {
  TRACE_EVENT0("startup", "CommonThreadManager::Init")
  InitializeMainThread();
  CreateThreads();
}

void CommonThreadManager::InitializeMainThread() {
  // Create a MessageLoop if one does not already exist for the current thread.
  if (!base::MessageLoop::current())
    main_message_loop_.reset(new base::MessageLoop(base::MessageLoop::TYPE_UI));

  const char* kThreadName = "Main";
  base::PlatformThread::SetName(kThreadName);
  if (main_message_loop_)
    main_message_loop_->set_thread_name(kThreadName);

  // Register the main thread by instantiating it, but don't call any methods.
  main_thread_.reset(
      new MainThreadType(CommonThread::UI, base::MessageLoop::current()));
  main_thread_->Init();
}

void CommonThreadManager::CreateThreads() {
  TRACE_EVENT0("startup", "CommonThreadManager::CreateThreads")

  TRACE_EVENT0("startup",
      "CommonThreadManager::MainMessageLoopStart:PreCreateThreads");
  result_code_ = PreCreateThreads();


  if (result_code_ > 0)
    return;

  base::Thread::Options default_options;
  base::Thread::Options io_message_loop_options;
  io_message_loop_options.message_loop_type = base::MessageLoop::TYPE_IO;
  base::Thread::Options ui_message_loop_options;
  ui_message_loop_options.message_loop_type = base::MessageLoop::TYPE_UI;

  // Start threads in the order they occur in the BrowserThread::ID
  // enumeration, except for BrowserThread::UI which is the main
  // thread.
  //
  // Must be size_t so we can increment it.
  for (size_t thread_id = CommonThread::UI + 1;
       thread_id < CommonThread::ID_COUNT;
       ++thread_id) {
    base::Thread::Options* options = &default_options;

    switch (thread_id) {
      case CommonThread::FILE:
#if defined(OS_WIN)
        // On Windows, the FILE thread needs to be have a UI message loop
        // which pumps messages in such a way that Google Update can
        // communicate back to us.
        options = &ui_message_loop_options;
#else
        options = &io_message_loop_options;
#endif
        break;
//       case CommonThread::CACHE:
//         options = &io_message_loop_options;
//         break;
//       case CommonThread::IO:
//         options = &io_message_loop_options;
//         break;
      case CommonThread::UI:
      case CommonThread::ID_COUNT:
        NOTREACHED();
        break;

      default:
        break;
    }
    CommonThread::ID id = static_cast<CommonThread::ID>(thread_id);

    TRACE_EVENT_BEGIN1("startup",
        "CommonThreadManager::CreateThreads:start",
        "Thread", id);

    threads_[id].reset(new ThreadType(id));
    threads_[id]->StartWithOptions(*options);

    TRACE_EVENT_END0("startup", "CommonThreadManager::CreateThreads:start");
  }

  TRACE_EVENT0("startup",
      "CommonThreadManager::CreateThreads:PreMainMessageLoopRun");
  PreMainMessageLoopRun();

  // If the UI thread blocks, the whole UI is unresponsive.
  // Do not allow disk IO from the UI thread.
  //base::ThreadRestrictions::SetIOAllowed(false);
  //base::ThreadRestrictions::DisallowWaiting();
}

void CommonThreadManager::ShutdownThreadsAndCleanUp() {
  // Teardown may start in PostMainMessageLoopRun, and during teardown we
  // need to be able to perform IO.
  //base::ThreadRestrictions::SetIOAllowed(true);
  //CommonThread::PostTask(
  //    CommonThread::IO, FROM_HERE,
  //    base::Bind(base::IgnoreResult(&base::ThreadRestrictions::SetIOAllowed),
  //               true));

  PostMainMessageLoopRun();

  // Must be size_t so we can subtract from it.
  for (size_t thread_id = CommonThread::ID_COUNT - 1;
       thread_id >= (CommonThread::UI + 1);
       --thread_id) {
    // Find the thread object we want to stop. Looping over all valid
    // BrowserThread IDs and DCHECKing on a missing case in the switch
    // statement helps avoid a mismatch between this code and the
    // BrowserThread::ID enumeration.
    //
    // The destruction order is the reverse order of occurrence in the
    // BrowserThread::ID list. The rationale for the order is as
    // follows (need to be filled in a bit):
    //
    //
    // - The IO thread is the only user of the CACHE thread.
    //
    // - The PROCESS_LAUNCHER thread must be stopped after IO in case
    //   the IO thread posted a task to terminate a process on the
    //   process launcher thread.
    //
    // - (Not sure why FILE needs to stop before WEBKIT.)
    //
    // - The WEBKIT thread (which currently is the responsibility of
    //   the embedder to stop, by destroying ResourceDispatcherHost
    //   before the DB thread is stopped)
    //
    // - (Not sure why DB stops last.)

    CommonThread::ID id = static_cast<CommonThread::ID>(thread_id);

    threads_[id].reset();
  }

  // Close the blocking I/O pool after the other threads. Other threads such
  // as the I/O thread may need to schedule work like closing files or flushing
  // data during shutdown, so the blocking pool needs to be available. There
  // may also be slow operations pending that will blcok shutdown, so closing
  // it here (which will block until required operations are complete) gives
  // more head start for those operations to finish.
  CommonThreadImpl::ShutdownThreadPool();

  PostDestroyThreads();

  main_thread_->CleanUp();
  main_thread_.reset();
  main_message_loop_.reset();
}

void CommonThreadManager::MainMessageLoopRun() {
  DCHECK_EQ(base::MessageLoop::TYPE_UI, base::MessageLoop::current()->type());

  base::RunLoop run_loop;
  run_loop.Run();
}

} // namespace common
