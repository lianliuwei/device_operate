#include "device/sequenced_bulk_buffer.h"

#include "base/location.h"
#include "base/bind.h"
#include "base/message_loop_proxy.h"

using namespace base;

bool SequencedBulkBufferBase::ReaderBase::WaitForReady() {
  CHECK(!IsQuit());

  bool ret = buffer_->CheckReady(Count(), 
      Bind(&SequencedBulkBufferBase::ReaderBase::GetBulk, Unretained(this)));
  // no need to wait if ready
  if (ret) {
    return !IsQuit();
  }
  buffer_->SetReadyCallback(this, Count(),
      Bind(&SequencedBulkBufferBase::ReaderBase::OnWaitReady, 
           Unretained(this)));
  event_.Wait();
  event_.Reset();
  return !IsQuit();
}

bool SequencedBulkBufferBase::ReaderBase::WaitTimeoutForReady(TimeDelta delta) {
  CHECK(!IsQuit());

  bool ret = buffer_->CheckReady(Count(),
      Bind(&SequencedBulkBufferBase::ReaderBase::GetBulk, Unretained(this)));
  // no need to wait if ready
  if (ret) {
    return !IsQuit();
  }
  buffer_->SetReadyCallback(this, Count(),
    base::Bind(&SequencedBulkBufferBase::ReaderBase::OnWaitReady, 
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

void SequencedBulkBufferBase::ReaderBase::OnWaitReady(int64 count, bool quit) {
  quiting_ = quit;
  if (!quit) {
    GetBulk();
  }
  event_.Signal();
}

void SequencedBulkBufferBase::ReaderBase::CallbackOnReady() {
  CHECK(!IsQuit());

  // HACK init the weak in the callback MessageLoop.
  // or OnBufferReady() will init it on wrong thread.
 
  base::SequencedTaskRunner* runner = MessageLoopProxy::current();
  DCHECK(runner) << "need runner for async OnBufferReady call";
  buffer_->SetReadyCallback(this, Count(),
      base::Bind(&SequencedBulkBufferBase::ReaderBase::OnBufferReady, 
                 Unretained(this),
                 runner,
                 weak_ptr_.GetWeakPtr()));
}

void SequencedBulkBufferBase::ReaderBase::OnBufferReady(SequencedTaskRunner* runner, 
                                                        WeakPtr<ReaderBase> weak_ptr,
                                                        int64 count, 
                                                        bool quit) {
  quiting_ = quit;
  if (!quit) {
    GetBulk();
  }
  if (!have_data_.is_null()) {
    runner->PostTask(FROM_HERE, 
        Bind(&SequencedBulkBufferBase::ReaderBase::CallHaveData, 
            // use weak_ptr_ so if reader is destroy when call callback.
            // the callback is no be called.
            weak_ptr));
  }
}


void SequencedBulkBufferBase::ReaderBase::CallHaveData() {
  if (!have_data_.is_null()) {
    have_data_.Run();
    // may be delete after this.
  }
}

void SequencedBulkBufferBase::Quit() {
 {
  base::AutoLock lock(lock_);

  DCHECK(quiting_ == false);
  quiting_ = true;
  }
  FireCallbackNoLock(CountNoLock());
}

void SequencedBulkBufferBase::SetReadyCallback(void* id, 
                                               int64 count, 
                                               BufferReadyCallback ready_callback) {
  {
  base::AutoLock lock(lock_);

  ReadyCallbackMap::iterator it = callback_map_.find(id);

  DCHECK(it == callback_map_.end()) << "one reader one callback";
  ReadyCallbackRecord record = { ready_callback, count, };
  callback_map_.insert(std::make_pair(static_cast<uint8*>(id), record));

  FireCallbackNoLock(CountNoLock());
  }
  }

bool SequencedBulkBufferBase::CancelCallback(void* id) {
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


void SequencedBulkBufferBase::FireCallbackNoLock(int64 reach_count) {
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

int64 SequencedBulkBufferBase::Count() const {
  base::AutoLock lock(lock_);
  return count_ - 1;
}


int64 SequencedBulkBufferBase::CountNoLock() const {
  return count_ - 1;
}

void SequencedBulkBufferBase::IncCount() {
  ++count_;
}

void SequencedBulkBufferBase::RegisterReader(ReaderBase* reader, int64 start_count) {
{
  base::AutoLock lock(lock_);
  // start_count mean no read yet
  int count = start_count - 1;

  // first one
  if (count_map_.size() == 0) {
    reader_front_count_ = reader_back_count_ = count;
  }

  ReaderCountMap::iterator it = count_map_.find(reader);
  DCHECK(it == count_map_.end());
  count_map_.insert(std::make_pair(reader, count));

  if (count > reader_back_count_) {
    reader_back_count_ = count;
  }
  if (count < reader_front_count_) {
    reader_front_count_ = count;
  }
  }
}

void SequencedBulkBufferBase::UnRegisterReader(ReaderBase* reader) {
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
  reader_front_count_ = min_value;
  reader_back_count_ = max_value;
  }
}

void SequencedBulkBufferBase::UpdateReaderCount(ReaderBase* reader, int64 count) {

  ReaderCountMap::iterator it = count_map_.find(reader);
  DCHECK(it != count_map_.end());
  if (it->second == count) {
    return;
  }
  DCHECK(it->second <= count);

  it->second = count;
  if (count > reader_back_count_) {
    reader_back_count_ = count;
  }
  int64 min_value = count;
  for (ReaderCountMap::iterator it = count_map_.begin();
       it != count_map_.end();
       ++it) {
    if (min_value > it->second) {
      min_value = it->second;
    }
  }
  reader_front_count_ = min_value;
}

int64 SequencedBulkBufferBase::ReaderMin() {
  return reader_front_count_;
}

int64 SequencedBulkBufferBase::ReaderMax() {
  return reader_back_count_;
}

int SequencedBulkBufferBase::ReaderNum() {
  return count_map_.size();
}

bool SequencedBulkBufferBase::GetReaderMinMax(int64* min_value, int64* max_value) {
  if (min_value) {
    *min_value = reader_front_count_;
  }
  if (max_value) {
    *max_value = reader_back_count_;
  }
  return count_map_.size() != 0;
}

SequencedBulkBufferBase::ReaderBase::ReaderBase(SequencedBulkBufferBase* buffer)
    : buffer_(buffer)
    , count_(0)
    , event_(true, false)
    , quiting_(false)
    , weak_ptr_(this) {
  DCHECK(buffer);
  buffer->RegisterReader(this, count_);
}

SequencedBulkBufferBase::ReaderBase::~ReaderBase() {
  buffer_->CancelCallback(this);
  buffer_->UnRegisterReader(this);
}

SequencedBulkBufferBase::SequencedBulkBufferBase()
    : reader_front_count_(-1)
    , reader_back_count_(-1)
    , count_(0)
    , quiting_(false) {

}

SequencedBulkBufferBase::~SequencedBulkBufferBase() {
  CHECK(count_map_.size() == 0);
  CHECK(callback_map_.size() == 0);
}
