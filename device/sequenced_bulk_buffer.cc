#include "device/sequenced_bulk_buffer.h"

#include "base/location.h"
#include "base/bind.h"
#include "base/message_loop_proxy.h"

using namespace base;

void SequencedBulkBufferBase::ReaderBase::WaitForReady() {
  CHECK(!IsQuit());

  bool ret = buffer_->CheckReady(Count());
  // no need to wait if ready
  if (ret) {
    return;
  }
  buffer_->SetReadyCallback(this, Count(),
      base::Bind(&SequencedBulkBufferBase::ReaderBase::OnWaitReady, 
                 base::Unretained(this)));
  event_.Wait();
  event_.Reset();
}

bool SequencedBulkBufferBase::ReaderBase::WaitTimeoutForReady(TimeDelta delta) {
  CHECK(!IsQuit());

  bool ret = buffer_->CheckReady(Count());
  // no need to wait if ready
  if (ret) {
    return true;
  }
  buffer_->SetReadyCallback(this, Count(),
    base::Bind(&SequencedBulkBufferBase::ReaderBase::OnWaitReady, 
    base::Unretained(this)));
  ret = event_.TimedWait(delta);
  if (!ret) {
    buffer_->CancelCallback(this);
    if (event_.IsSignaled()) {
      event_.Reset();
    }
  } else {
    event_.Reset();
  }
  return ret;
}

void SequencedBulkBufferBase::ReaderBase::OnWaitReady(int64 count, bool quit) {
  quiting_ = quit;
  event_.Signal();
}

void SequencedBulkBufferBase::ReaderBase::CallbackOnReady() {
  CHECK(!IsQuit());

  base::SequencedTaskRunner* runner = MessageLoopProxy::current();
  DCHECK(runner) << "need runner for async OnBufferReady call";
  buffer_->SetReadyCallback(this, Count(),
      base::Bind(&SequencedBulkBufferBase::ReaderBase::OnBufferReady, 
                 base::Unretained(this),
                 runner));
}

void SequencedBulkBufferBase::ReaderBase::OnBufferReady(SequencedTaskRunner* runner, 
                                                        int64 count, 
                                                        bool quit) {
  quiting_ = quit;
 
  if (!have_data_.is_null()) {
    runner->PostTask(FROM_HERE, have_data_);
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


bool SequencedBulkBufferBase::CheckReady(int64 count) {
  base::AutoLock lock(lock_);
  return count <= CountNoLock();
}

void SequencedBulkBufferBase::SetReadyCallback(void* id, 
                                               int64 count, 
                                               BufferReadyCallback ready_callback) {
  {
  base::AutoLock lock(lock_);

  ReadyCallbackMap::iterator it = callback_map_.find(id);

  DCHECK(it == callback_map_.end());
  ReadyCallbackRecord record = { ready_callback, count, };
  callback_map_.insert(std::make_pair(static_cast<uint8*>(id), record));

  FireCallbackNoLock(CountNoLock());
  }
  }

void SequencedBulkBufferBase::CancelCallback(void* id) {
  {
  base::AutoLock lock(lock_);

  ReadyCallbackMap::iterator it = callback_map_.find(id);

  // callback may just fire, before Cancelcallback
  if (it == callback_map_.end()) {
    return;
  }
  callback_map_.erase(it);
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
    , quiting_(false) {
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
