#pragma once

#include <map>
#include <list>

#include "base/memory/ref_counted.h"
#include "base/time.h"
#include "base/callback.h"
#include "base/logging.h"
#include "base/sequenced_task_runner.h"
#include "base/synchronization/waitable_event.h"
#include "base/synchronization/lock.h"

// TODO need restart push bulk after bulk full.
// this add in memory pool callback.
// void ReStartPush();

class SequencedBulkBufferBase : public base::RefCountedThreadSafe<SequencedBulkBufferBase> {
  typedef base::Callback<void(void)> BufferHaveDataCallback;

public:
  class ReaderBase {
  public:
    // the count need to read
    int64 Count() const { return count_; }
    void Skip(int64 n) { count_ += n; }

    // IMPORTANT: after wait GetBulk() may fault, 
    // because the bulk may be free or recycle.
    // sync wait
    void WaitForReady();
    // true for finish false for timeout
    bool WaitTimeoutForReady(base::TimeDelta delta);

    // async
    void CallbackOnReady();

    // can not change when in async wait. 
    // callback no nest in OnBufferReady()
    void set_have_data_callback(BufferHaveDataCallback have_data) {
      have_data_ = have_data;
    }

    bool IsQuit() const { return quiting_; }

  protected:
    ReaderBase(SequencedBulkBufferBase* buffer);
    ~ReaderBase();

    void set_count(int64 n) { count_ = n; }

    scoped_refptr<SequencedBulkBufferBase> buffer_;

  private:
    // for sync
    void OnWaitReady(int64 count, bool quit);

    // for async
    void OnBufferReady(base::SequencedTaskRunner* runner, int64 count, bool quit);

    BufferHaveDataCallback have_data_;
    int64 count_;
    base::WaitableEvent event_;
    bool quiting_;

    DISALLOW_COPY_AND_ASSIGN(ReaderBase);
  };

  SequencedBulkBufferBase();
  
  // lock
  void Quit();

  // current count, -1 for no ready bulk yet
  // lock
  int64 Count() const;

protected:
  friend class base::RefCountedThreadSafe<SequencedBulkBufferBase>;
  virtual ~SequencedBulkBufferBase();

  // all protected no lock
  // inc only
  void UpdateReaderCount(ReaderBase* reader, int64 count);

  void IncCount();

  int64 CountNoLock() const;

  // 0 the Min Max is invalid
  int ReaderNum();
  int64 ReaderMin();
  int64 ReaderMax();

  // return if min max valid
  bool GetReaderMinMax(int64* min_value, int64* max_value);


  void FireCallbackNoLock(int64 reach_count);

  mutable base::Lock lock_;

private:
  friend class ReaderBase;

  // void OnBufferReady(int64 count, bool quit);
  typedef base::Callback<void(int64, bool)> BufferReadyCallback;

  // lock
  void RegisterReader(ReaderBase* reader, int64 start_count);
  // lock
  void UnRegisterReader(ReaderBase* reader);

  // one time callback.
  // if callback match, call immediate.
  // lock
  void SetReadyCallback(void* id, int64 count, BufferReadyCallback ready_callback);
  // lock
  void CancelCallback(void* id);
  // lock
  bool CheckReady(int64 count);


  bool quiting_;

  // total count
  int64 count_;

  struct ReadyCallbackRecord {
    BufferReadyCallback callback;
    int64 count;
  };

  // register callback when count reach
  typedef std::map<void*, ReadyCallbackRecord> ReadyCallbackMap;
  ReadyCallbackMap callback_map_;

  // each reader progress
  typedef std::map<ReaderBase*, int64> ReaderCountMap;
  ReaderCountMap count_map_;

  // reader current_count_ range
  int64 reader_front_count_;
  int64 reader_back_count_;

  DISALLOW_COPY_AND_ASSIGN(SequencedBulkBufferBase);
};


template<typename BulkPtrType>
struct BulkPiece {
  BulkPtrType bulk;
  int64 count;
};

// use ref_count ptr type
template<typename BulkPtrType>
class SequencedBulkBuffer : public SequencedBulkBufferBase {
public:
  typedef BulkPiece<BulkPtrType> BulkPieceType;

  // sync reader method use in another thread.
  class Reader : public ReaderBase {
  public:
    Reader(SequencedBulkBuffer* seq) : ReaderBase(seq) {}
    ~Reader() {}

    // return true if bluck get, bluck_count >= last_count
    // if bluck_count > last_count mean data lost by overwrite.
    bool GetBluk(BulkPtrType* bluck_ptr, int64* bluck_count);

  private: 
    SequencedBulkBuffer* buffer() { 
      return static_cast<SequencedBulkBuffer*>(buffer_.get());
    }

    DISALLOW_COPY_AND_ASSIGN(Reader);
  };

  SequencedBulkBuffer(bool over_write, bool keep_if_no_reader)
    : over_write_(over_write)
    , keep_if_no_reader_(keep_if_no_reader) {}

  bool GetBluk(Reader* reader, BulkPtrType* bluck_ptr, int64* bluck_count, int64 count);
  
  // write method
  // this will inc bulk count.
  void PushBluk(BulkPtrType bulk);

  int bulk_num() { 
    base::AutoLock lock(lock_);
    return buffer_queue_.size(); 
  }

  // recycle one is for over-write mode, recycle no be read buffer first
  // and recycle between reader buffer then,
  // if all is occupy, means need more buffer, or need process faster.
  bool RecycleOne();

private:
  // these method no lock
  // 
  // Release the buffer the all reader read.
  void MayReleaseBuffer();
  // find bulk which have the most small of the bigger or equal count 
  bool FindBulk(BulkPtrType* bluck_ptr, int64* bluck_count, int64 count);

  bool over_write_;

  bool keep_if_no_reader_;

  std::list<BulkPieceType> buffer_queue_;


  DISALLOW_COPY_AND_ASSIGN(SequencedBulkBuffer);
};

template<typename BulkPtrType>
bool SequencedBulkBuffer<BulkPtrType>::RecycleOne() {
  base::AutoLock lock(lock_);

  DCHECK(over_write_);
  int64 min_value;
  int64 max_value;
  bool valid = GetReaderMinMax(&min_value, &max_value);
  // no reader by all bulk may be ref in process module
  if (!valid) {
    return false;
  }
  // recycle no be reader first
  std::list<BulkPieceType>::iterator it = buffer_queue_.begin();
  while(it != buffer_queue_.end()) {
    if (it->count > max_value) {
      it->bulk = NULL;
      buffer_queue_.erase(it);
      return true;
    }
    ++it;
  }
  it = buffer_queue_.begin();
  while(it != buffer_queue_.end()) {
    if (it->count > min_value && it->count <= max_value) {
      it->bulk = NULL;
      buffer_queue_.erase(it);
      return true;
    }
    ++it;
  }
  return false;
}


template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::MayReleaseBuffer() {
  int64 min_value;
  bool release_all = !GetReaderMinMax(&min_value, NULL);
  if (release_all && keep_if_no_reader_) {
    return;
  }
  std::list<BulkPieceType>::iterator it = buffer_queue_.begin();
  while(it != buffer_queue_.end()) {
    if (release_all) {
      it->bulk = NULL;
      std::list<BulkPieceType>::iterator remove_it = it;
      ++it;
      buffer_queue_.erase(remove_it);
    } else if (it->count <= min_value) {
      it->bulk = NULL;
      std::list<BulkPieceType>::iterator remove_it = it;
      ++it;
      buffer_queue_.erase(remove_it);
    } else {
      break;
    }
  }
}


template<typename BulkPtrType>
bool SequencedBulkBuffer<BulkPtrType>::GetBluk(Reader* reader, 
                                               BulkPtrType* bluck_ptr, int64* bluck_count, 
                                               int64 count) {
  {
  base::AutoLock lock(lock_);

  BulkPtrType out_ptr;
  int64 out_count;
  bool ret = FindBulk(&out_ptr, &out_count, count);
  if (ret) {
    // auto release no used buffer
    UpdateReaderCount(reader, out_count);
    MayReleaseBuffer();

    if (bluck_ptr) {
      *bluck_ptr = out_ptr;
    }
    if (bluck_count) {
      *bluck_count = out_count;
    }
  }
  return ret;
  }
}


template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::PushBluk(BulkPtrType bulk_ptr) {
  {
  base::AutoLock lock(lock_);
  BulkPieceType bulk  = { bulk_ptr, CountNoLock() + 1};
  buffer_queue_.push_back(bulk);
  IncCount();
  FireCallbackNoLock(CountNoLock());
  // if no reader, just release it.
  MayReleaseBuffer();
  }
}

template<typename BulkPtrType>
bool SequencedBulkBuffer<BulkPtrType>::FindBulk(BulkPtrType* bluck_ptr, 
                                                int64* bluck_count, 
                                                int64 count) {
  // bulk no in queue now
  if (CountNoLock() < count) {
    return false;
  }
  for (std::list<BulkPieceType>::iterator it = buffer_queue_.begin();
       it != buffer_queue_.end();
       ++it) {
    if (it->count >= count) {
      *bluck_ptr = it->bulk;
      *bluck_count = it->count;
      return true;
    }
  }
  return false;
}

template<typename BulkPtrType>
bool SequencedBulkBuffer<BulkPtrType>::Reader::GetBluk(BulkPtrType* bluck_ptr, 
                                                       int64* bluck_count) {
  BulkPtrType out_ptr;
  int64 out_count;
  bool ret = buffer()->GetBluk(this, &out_ptr, &out_count, Count());
  if (!ret) {
    return false;
  }
  // update count.
  DCHECK(Count() <= out_count);
  set_count(out_count + 1);

  if (bluck_ptr) {
    *bluck_ptr = out_ptr;
  }
  if (bluck_count) {
    *bluck_count = out_count;
  }

  return true;
}

