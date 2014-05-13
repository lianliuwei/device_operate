#include "device/sequenced_bulk_queue.h"

#include <vector>

#include "base/location.h"
#include "base/bind.h"
#include "base/message_loop_proxy.h"
#include "base/threading/thread_restrictions.h"

using namespace base;

namespace {
// return 0 mean timeout
size_t TimedWaitMany(WaitableEvent** events, size_t count, const TimeDelta& max_time) {
  base::ThreadRestrictions::AssertWaitAllowed();
  HANDLE handles[MAXIMUM_WAIT_OBJECTS];
  CHECK_LE(static_cast<int>(count), MAXIMUM_WAIT_OBJECTS)
      << "Can only wait on " << MAXIMUM_WAIT_OBJECTS << " with WaitMany";
  DCHECK(max_time >= TimeDelta::FromMicroseconds(0));
  
  // Be careful here.  TimeDelta has a precision of microseconds, but this API
  // is in milliseconds.  If there are 5.5ms left, should the delay be 5 or 6?
  // It should be 6 to avoid returning too early.
  double timeout = ceil(max_time.InMillisecondsF());

  for (size_t i = 0; i < count; ++i)
    handles[i] = events[i]->handle();

  // The cast is safe because count is small - see the CHECK above.
  DWORD result =
      WaitForMultipleObjects(static_cast<DWORD>(count),
                             handles,
                             FALSE,      // don't wait for all the objects
                             static_cast<DWORD>(timeout));
  if (result == WAIT_TIMEOUT) {
    return 0;
  } else if (result >= WAIT_OBJECT_0 + count) {
    DLOG_GETLASTERROR(FATAL) << "WaitForMultipleObjects failed";
    return 0;
  }

  return result - WAIT_OBJECT_0;
}

}


void SequencedBulkQueueBase::WaitForManyReader::Wait() {
  DCHECK(wait_list_.size());
  bool ret;
  // new wait result
  finish_list_.clear();
  std::set<ReaderBase*> need_check;
  for (std::set<ReaderBase*>::iterator it = wait_list_.begin();
       it != wait_list_.end();
       ++it) {
    ret = (*it)->WaitFront();
    if (ret) {
      finish_list_.insert(*(it));
    } else {
      need_check.insert(*(it));
    }
  }
  if (need_check.size() == 0) {
    return;
  }
  std::vector<WaitableEvent*> event_list;
  event_list.reserve(need_check.size());
  for (std::set<ReaderBase*>::iterator it = need_check.begin();
       it != need_check.end();
       ++it) {
      event_list.push_back(&((*it)->event_));
  }
  size_t index = WaitableEvent::WaitMany(&(event_list[0]), event_list.size());
  for (std::set<ReaderBase*>::iterator it = need_check.begin();
       it != need_check.end();
       ++it) {
    ret = (*it)->WaitBack();
    if (ret) {
      finish_list_.insert(*(it));
    }
  }
}

bool SequencedBulkQueueBase::WaitForManyReader::WaitTimeout(base::TimeDelta delta) {
  DCHECK(wait_list_.size());
  bool ret;
  // new wait result
  finish_list_.clear();
  std::set<ReaderBase*> need_check;
  for (std::set<ReaderBase*>::iterator it = wait_list_.begin();
       it != wait_list_.end();
       ++it) {
    ret = (*it)->WaitFront();
    if (ret) {
      finish_list_.insert(*(it));
    } else {
      need_check.insert(*(it));
    }
  }
  if (need_check.size() == 0) {
    return true;
  }
  std::vector<WaitableEvent*> event_list;
  event_list.reserve(need_check.size());
  for (std::set<ReaderBase*>::iterator it = need_check.begin();
       it != need_check.end();
       ++it) {
      event_list.push_back(&((*it)->event_));
  }

  size_t index = TimedWaitMany(&(event_list[0]), event_list.size(), delta);
  for (std::set<ReaderBase*>::iterator it = need_check.begin();
       it != need_check.end();
       ++it) {
    ret = (*it)->WaitBack();
    if (ret) {
      finish_list_.insert(*(it));
    }
  }
  return finish_list_.size() != 0;
}

bool SequencedBulkQueueBase::ReaderBase::WaitFront() {
  CHECK(!IsQuit());

  bool ret = buffer_->CheckReady(Count(), 
    Bind(&SequencedBulkQueueBase::ReaderBase::GetBulk, Unretained(this)));
  // no need to wait if ready
  if (ret) {
    return true;
  }
  buffer_->SetReadyCallback(this, Count(),
    Bind(&SequencedBulkQueueBase::ReaderBase::OnWaitReady, 
    Unretained(this)));
 return false;
}

bool SequencedBulkQueueBase::ReaderBase::WaitBack() {
  bool called = !buffer_->CancelCallback(this);
  // be call in TimeWait() false and CancelCallback()
  if (called) {
    event_.Wait();
    event_.Reset();
    return true;
  } else {
    event_.Reset();
    return false;
  }
}

bool SequencedBulkQueueBase::ReaderBase::WaitForReady() {
  CHECK(!IsQuit());

  bool ret = buffer_->CheckReady(Count(), 
      Bind(&SequencedBulkQueueBase::ReaderBase::GetBulk, Unretained(this)));
  // no need to wait if ready
  if (ret) {
    return true;
  }
  buffer_->SetReadyCallback(this, Count(),
      Bind(&SequencedBulkQueueBase::ReaderBase::OnWaitReady, 
           Unretained(this)));
  event_.Wait();
  event_.Reset();
  return !IsQuit();
}

bool SequencedBulkQueueBase::ReaderBase::WaitTimeoutForReady(TimeDelta delta) {
  CHECK(!IsQuit());

  bool ret = buffer_->CheckReady(Count(),
      Bind(&SequencedBulkQueueBase::ReaderBase::GetBulk, Unretained(this)));
  // no need to wait if ready
  if (ret) {
    return true;
  }
  buffer_->SetReadyCallback(this, Count(),
    base::Bind(&SequencedBulkQueueBase::ReaderBase::OnWaitReady, 
    base::Unretained(this)));
  ret = event_.TimedWait(delta);
  if (!ret) {
    bool called = !buffer_->CancelCallback(this);
    // be call in TimeWait() false and CancelCallback()
    if (called) {
      event_.Wait();
      event_.Reset();
      return !IsQuit();
    }
    return false;
    // normal event trigger
  } else {
    event_.Reset();
    return !IsQuit();
  }
}

void SequencedBulkQueueBase::ReaderBase::OnWaitReady(int64 count, bool quit) {
  quiting_ = quit;
  if (!quit) {
    GetBulk();
  }
  event_.Signal();
}

void SequencedBulkQueueBase::ReaderBase::CallbackOnReady() {
  CHECK(!IsQuit());

  // HACK init the weak in the callback MessageLoop.
  // or OnBufferReady() will init it on wrong thread.
 
  base::SequencedTaskRunner* runner = MessageLoopProxy::current();
  DCHECK(runner) << "need runner for async OnBufferReady call";
  buffer_->SetReadyCallback(this, Count(),
      base::Bind(&SequencedBulkQueueBase::ReaderBase::OnBufferReady, 
                 Unretained(this),
                 runner,
                 weak_ptr_.GetWeakPtr()));
}

void SequencedBulkQueueBase::ReaderBase::OnBufferReady(SequencedTaskRunner* runner, 
                                                        WeakPtr<ReaderBase> weak_ptr,
                                                        int64 count, 
                                                        bool quit) {
  quiting_ = quit;
  if (!quit) {
    GetBulk();
  }
  if (!have_data_.is_null()) {
    runner->PostTask(FROM_HERE, 
        Bind(&SequencedBulkQueueBase::ReaderBase::CallHaveData, 
            // use weak_ptr_ so if reader is destroy when call callback.
            // the callback is no be called.
            weak_ptr));
  }
}


void SequencedBulkQueueBase::ReaderBase::CallHaveData() {
  if (!have_data_.is_null()) {
    have_data_.Run();
    // may be delete after this.
  }
}

void SequencedBulkQueueBase::Quit() {
 {
  base::AutoLock lock(lock_);

  DCHECK(quiting_ == false);
  quiting_ = true;
  }
  FireCallbackNoLock(CountNoLock());
}

void SequencedBulkQueueBase::SetReadyCallback(void* id, 
                                               int64 count, 
                                               BufferReadyCallback ready_callback) {
  {
  base::AutoLock lock(lock_);

  ReadyCallbackMap::iterator it = callback_map_.find(id);

  DCHECK(it == callback_map_.end()) << "one reader one callback";
  ReadyCallbackRecord record = { ready_callback, count, };
  callback_map_.insert(std::make_pair(static_cast<uint8*>(id), record));
  }
  }

bool SequencedBulkQueueBase::CancelCallback(void* id) {
  {
  base::AutoLock lock(lock_);

  ReadyCallbackMap::iterator it = callback_map_.find(id);

  // callback may just fire, before Cancelcallback
  if (it == callback_map_.end()) {
    return false;
  }
  callback_map_.erase(it);
  return true;
  }
}


void SequencedBulkQueueBase::FireCallbackNoLock(int64 reach_count) {
  if (quiting_) {
    for (ReadyCallbackMap::iterator it = callback_map_.begin();
      it != callback_map_.end();
      ++it) {
        it->second.callback.Run(reach_count, true);
    }
    return;
  }

  ReadyCallbackMap::iterator it = callback_map_.begin();
  while (it != callback_map_.end()) {
    if (it->second.count <= reach_count) {
      it->second.callback.Run(reach_count, false);
      ReadyCallbackMap::iterator remove_it = it;
      ++it;
      callback_map_.erase(remove_it);
      continue;
    }
    ++it;
  }
}

int64 SequencedBulkQueueBase::Count() const {
  base::AutoLock lock(lock_);
  return count_ - 1;
}


int64 SequencedBulkQueueBase::CountNoLock() const {
  return count_ - 1;
}

void SequencedBulkQueueBase::IncCount() {
  ++count_;
}

void SequencedBulkQueueBase::RegisterReader(ReaderBase* reader, int64 start_count) {
{
  base::AutoLock lock(lock_);
  // start_count mean no read yet
  int count = start_count - 1;

  // first one
  if (count_map_.size() == 0) {
    reader_min_ = reader_max_ = count;
  }

  ReaderCountMap::iterator it = count_map_.find(reader);
  DCHECK(it == count_map_.end());
  count_map_.insert(std::make_pair(reader, count));

  if (count > reader_max_) {
    reader_max_ = count;
  }
  if (count < reader_min_) {
    reader_min_ = count;
  }
  }
}

void SequencedBulkQueueBase::UnRegisterReader(ReaderBase* reader) {
  {
  base::AutoLock lock(lock_);

  ReaderCountMap::iterator it = count_map_.find(reader);
  DCHECK(it != count_map_.end());
  count_map_.erase(it);

  // least one
  if (count_map_.size() == 0)
    return;

  int64 min_value = (count_map_.begin())->second;
  int64 max_value = min_value;
  for (ReaderCountMap::iterator it = count_map_.begin();
    it != count_map_.end();
    ++it) {
      if (min_value > it->second) {
        min_value = it->second;
      }
      if (max_value < it->second) {
        max_value = it->second;
      }
  }
  reader_min_ = min_value;
  reader_max_ = max_value;
  }
}

void SequencedBulkQueueBase::UpdateReaderCount(ReaderBase* reader, int64 count) {

  ReaderCountMap::iterator it = count_map_.find(reader);
  DCHECK(it != count_map_.end());
  if (it->second == count) {
    return;
  }
  DCHECK(it->second <= count);

  it->second = count;
  if (count > reader_max_) {
    reader_max_ = count;
  }
  int64 min_value = count;
  for (ReaderCountMap::iterator it = count_map_.begin();
       it != count_map_.end();
       ++it) {
    if (min_value > it->second) {
      min_value = it->second;
    }
  }
  reader_min_ = min_value;
}

int64 SequencedBulkQueueBase::ReaderMin() {
  return reader_min_;
}

int64 SequencedBulkQueueBase::ReaderMax() {
  return reader_max_;
}

int SequencedBulkQueueBase::ReaderNum() {
  return count_map_.size();
}

bool SequencedBulkQueueBase::GetReaderMinMax(int64* min_value, int64* max_value) {
  if (min_value) {
    *min_value = reader_min_;
  }
  if (max_value) {
    *max_value = reader_max_;
  }
  return count_map_.size() != 0;
}

SequencedBulkQueueBase::ReaderBase::ReaderBase(SequencedBulkQueueBase* buffer)
    : buffer_(buffer)
    , count_(0)
    , event_(true, false)
    , quiting_(false)
    , weak_ptr_(this) {
  DCHECK(buffer);
  buffer->RegisterReader(this, count_);
}

SequencedBulkQueueBase::ReaderBase::~ReaderBase() {
  buffer_->CancelCallback(this);
  buffer_->UnRegisterReader(this);
}

SequencedBulkQueueBase::SequencedBulkQueueBase()
    : reader_min_(-1)
    , reader_max_(-1)
    , count_(0)
    , quiting_(false) {

}

SequencedBulkQueueBase::~SequencedBulkQueueBase() {
  CHECK(count_map_.size() == 0);
  CHECK(callback_map_.size() == 0);
}
