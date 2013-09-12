#pragma once

#include "base/basictypes.h"
#include "base/values.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/ref_counted.h"
#include "base/observer_list.h"
#include "base/message_loop_proxy.h"
#include "base/synchronization/lock.h"

class AsyncTask;
enum AsyncTaskStatus;

// param belong to AsyncTask
class AsyncTaskObserver {
public:
  // notify task Start
  virtual void OnAsyncTaskStart(AsyncTask* task, 
      base::Value* param) {}
  
  // task either Finish or Cancel or Error
  // user can release the ref to UITask. 
  virtual void OnAsyncTaskFinish(AsyncTask* task, AsyncTaskStatus status, 
      base::Value* param) {}

  // task progress
  virtual void OnAsyncTaskProgress(AsyncTask* task, int percent, 
      base::Value* param) {}

protected:
  virtual ~AsyncTaskObserver() {}
};

enum AsyncTaskStatus {
  kNoStart,
  kRunning,
  kError,
  kCancel,
  kFinish,
};

// represent the async task, the status change will post to the create task thread
// the callback is run in the task thread. use AsyncTaskObserver to get status
// change notify.
// pass by AsyncTaskHandle
class AsyncTask : public base::RefCountedThreadSafe<AsyncTask> {
public:
  AsyncTask();
  ~AsyncTask() {}

  
  AsyncTaskStatus status();

  bool IsReady() {return status() == kNoStart; }
  bool IsRunning() {return status() == kRunning; }
  bool IsError() {return status() == kError; }
  bool IsCancel() {return status() == kCancel; }
  bool IsFinish() {return status() == kFinish; }
 
  // 0 - 100
  int GetProgress();

  // cancel task
  void Cancel();

  bool UserCancel();

  void AddObserver(AsyncTaskObserver* obs);
  void RemoveObserver(AsyncTaskObserver* obs);
  bool HasObserver(AsyncTaskObserver* obs);

  // call on working thread
  // take own of param
  void NotifyStart(base::Value* param);
  // if add, add percent to percent_ if no set percent to percent_
  void NotifyProgress(bool add, double percent, base::Value* param);
  void NotifyError(base::Value* param);
  void NotifyFinish(base::Value* param);
  void NotifyCancel(base::Value* param);

private:
  void OnNotifyProgress(double percent, base::Value* param);

  void NotifyFinishImp(AsyncTaskStatus status, base::Value* param);

  ObserverList<AsyncTaskObserver> observer_list_;

  // bind message_loop_
  scoped_refptr<base::MessageLoopProxy> message_loop_;

  // protect follow member
  base::Lock lock_;
  AsyncTaskStatus status_;
  double percent_;
  // flag the work thread to Cancel.
  bool cancel_;
  
  DISALLOW_COPY_AND_ASSIGN(AsyncTask);
};

typedef scoped_refptr<AsyncTask> AsyncTaskHandle;
