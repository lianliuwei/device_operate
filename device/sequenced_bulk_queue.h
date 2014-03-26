#pragma once

#include <map>
#include <list>
#include <set>

#include "base/memory/ref_counted.h"
#include "base/time.h"
#include "base/callback.h"
#include "base/logging.h"
#include "base/sequenced_task_runner.h"
#include "base/synchronization/waitable_event.h"
#include "base/synchronization/lock.h"
#include "base/memory/weak_ptr.h"

// TODO need restart push bulk after bulk full.
// this add in memory pool callback.
// void ReStartPush();

class SequencedBulkQueueBase : public base::RefCountedThreadSafe<SequencedBulkQueueBase> {
  typedef base::Callback<void(void)> BufferHaveDataCallback;

public:
  class ReaderBase;

  class WaitForManyReader {
  public:
    WaitForManyReader() {}
    ~WaitForManyReader() {}

    void AddReader(ReaderBase* reader) { wait_list_.insert(reader); }
    void RemoveReader(ReaderBase* reader) { wait_list_.erase(reader); }

    void Wait();

    // return true wait success, return false timeout
    bool WaitTimeout(base::TimeDelta delta);

    // last wait result, finish include quit
    bool IsReaderFinish(ReaderBase* reader) { 
      return finish_list_.find(reader) != finish_list_.end(); }

  private:
    std::set<ReaderBase*> wait_list_;
    std::set<ReaderBase*> finish_list_;
  };

  class ReaderBase {
  public:
    // the count need to read
    int64 Count() const { return count_; }
    void Skip(int64 n) { count_ += n; }


    // can not change when in async wait.
    // callback no nest in OnBufferReady()
    void set_have_data_callback(BufferHaveDataCallback have_data) {
      have_data_ = have_data;
    }

    bool IsQuit() const { return quiting_; }

    // async
    void CallbackOnReady();

  protected:
    ReaderBase(SequencedBulkQueueBase* buffer);
    ~ReaderBase();

    void set_count(int64 n) { count_ = n; }

    scoped_refptr<SequencedBulkQueueBase> buffer_;

    // this call in Lock
    virtual void GetBulk() = 0;

    // call GetBulk when ready
    // sync wait
    bool WaitForReady();
    // true for finish false for timeout
    bool WaitTimeoutForReady(base::TimeDelta delta);

  private:
    // for sync
    void OnWaitReady(int64 count, bool quit);

    // for async
    void OnBufferReady(base::SequencedTaskRunner* runner, 
                        // pass weak_ptr, so all weak_ptr flag create on task thread
                        base::WeakPtr<ReaderBase> weak_ptr, 
                        int64 count, bool quit);

    void CallHaveData();

    // for wait more then one reader
    friend class WaitForManyReader;
    bool WaitFront();
    bool WaitBack();

    base::WeakPtrFactory<ReaderBase> weak_ptr_;
    BufferHaveDataCallback have_data_;
    int64 count_;
    base::WaitableEvent event_;
    bool quiting_;

    DISALLOW_COPY_AND_ASSIGN(ReaderBase);
  };

  SequencedBulkQueueBase();

  // lock
  void Quit();

  // current count, -1 for no ready bulk yet
  // lock
  int64 Count() const;

protected:
  friend class base::RefCountedThreadSafe<SequencedBulkQueueBase>;
  virtual ~SequencedBulkQueueBase();

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
  // return true if callback no call yet
  // false if callback is fire already
  bool CancelCallback(void* id);
  // lock
  // have bulk that count >= count.
  // call ready when return true
  virtual bool CheckReady(int64 count, const base::Closure& ready) = 0;


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
  int64 reader_min_;
  int64 reader_max_;

  DISALLOW_COPY_AND_ASSIGN(SequencedBulkQueueBase);
};


template<typename BulkPtrType>
struct BulkPiece {
  BulkPtrType bulk;
  int64 count;
};

// use ref_count ptr type
template<typename BulkPtrType>
class SequencedBulkQueue : public SequencedBulkQueueBase {
public:
  typedef BulkPiece<BulkPtrType> BulkPieceType;

  // sync reader method use in another thread.
  class Reader : public ReaderBase {
  public:
    Reader(SequencedBulkQueue* seq) : ReaderBase(seq) {}
    ~Reader() {}

    // return true if bulk get, bulk_count >= last_count
    // if buck_count > last_count mean data lost by overwrite.
    bool GetBulkSameThread(BulkPtrType* bulk_ptr, int64* bulk_count);

    // call in other thread
    // false when need to quit
    bool WaitGetBulk(BulkPtrType* bulk_ptr, int64* bulk_count);

    // call in other thread
    // return false mean timeout or quit, ptr and count are no touch
    bool WaitTimeoutGetBulk(BulkPtrType* bulk_ptr, int64* bulk_count, base::TimeDelta delta);

    // Async callback get Result
    bool GetResult(BulkPtrType* bulk_ptr, int64* bulk_count);

  private:
    SequencedBulkQueue* buffer() {
      return static_cast<SequencedBulkQueue*>(buffer_.get());
    }

    // implement ReaderBase
    virtual void GetBulk();

    BulkPtrType temp_ptr_;
    int64 temp_count_;

    DISALLOW_COPY_AND_ASSIGN(Reader);
  };

  SequencedBulkQueue(bool over_write, bool keep_if_no_reader)
    : over_write_(over_write)
    , keep_if_no_reader_(keep_if_no_reader) {}

  bool GetBulk(Reader* reader, BulkPtrType* bulk_ptr, int64* bulk_count, int64 count);
  bool GetBulkNoLock(Reader* reader, BulkPtrType* bulk_ptr, int64* bulk_count, int64 count);
  // write method
  // this will inc bulk count.
  void PushBulk(BulkPtrType bulk);

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
  bool FindBulk(BulkPtrType* bulk_ptr, int64* bulk_count, int64 count);

  bool over_write_;

  bool keep_if_no_reader_;

  std::list<BulkPieceType> buffer_queue_;

  // implement SequencedBulkBufferBase
  virtual bool CheckReady(int64 count, const base::Closure& ready);

  DISALLOW_COPY_AND_ASSIGN(SequencedBulkQueue);
};

template<typename BulkPtrType>
void SequencedBulkQueue<BulkPtrType>::Reader::GetBulk() {
  bool ret = buffer()->GetBulkNoLock(this, &temp_ptr_, &temp_count_, Count());
  DCHECK(ret);
  // update count.
  DCHECK(Count() <= temp_count_);
  set_count(temp_count_ + 1);
}

template<typename BulkPtrType>
bool SequencedBulkQueue<BulkPtrType>::RecycleOne() {
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
void SequencedBulkQueue<BulkPtrType>::MayReleaseBuffer() {
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
bool SequencedBulkQueue<BulkPtrType>::GetBulk(Reader* reader,
                                              BulkPtrType* bulk_ptr, int64* bulk_count,
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

    if (bulk_ptr) {
      *bulk_ptr = out_ptr;
    }
    if (bulk_count) {
      *bulk_count = out_count;
    }
  }
  return ret;
  }
}

template<typename BulkPtrType>
bool SequencedBulkQueue<BulkPtrType>::GetBulkNoLock(Reader* reader,
                                                    BulkPtrType* bulk_ptr, int64* bulk_count,
                                                    int64 count) {
  BulkPtrType out_ptr;
  int64 out_count;
  bool ret = FindBulk(&out_ptr, &out_count, count);
  if (ret) {
    // auto release no used buffer
    UpdateReaderCount(reader, out_count);
    MayReleaseBuffer();

    if (bulk_ptr) {
      *bulk_ptr = out_ptr;
    }
    if (bulk_count) {
      *bulk_count = out_count;
    }
  }
  return ret;
}


template<typename BulkPtrType>
bool SequencedBulkQueue<BulkPtrType>::Reader::GetResult(BulkPtrType* bulk_ptr, 
                                                        int64* bulk_count) {
  if (IsQuit()) {
    return false;
  }
  if (bulk_ptr) {
    *bulk_ptr = temp_ptr_;
  }
  if (bulk_count) {
    *bulk_count = temp_count_;
  }
  return true;
}

template<typename BulkPtrType>
void SequencedBulkQueue<BulkPtrType>::PushBulk(BulkPtrType bulk_ptr) {
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
bool SequencedBulkQueue<BulkPtrType>::FindBulk(BulkPtrType* bulk_ptr,
                                                int64* bulk_count,
                                                int64 count) {
  // bulk no in queue now
  if (CountNoLock() < count) {
    return false;
  }
  for (std::list<BulkPieceType>::iterator it = buffer_queue_.begin();
       it != buffer_queue_.end();
       ++it) {
    if (it->count >= count) {
      *bulk_ptr = it->bulk;
      *bulk_count = it->count;
      return true;
    }
  }
  return false;
}

template<typename BulkPtrType>
bool SequencedBulkQueue<BulkPtrType>::Reader::GetBulkSameThread(BulkPtrType* bulk_ptr,
                                                                 int64* bulk_count) {
  BulkPtrType out_ptr;
  int64 out_count;
  bool ret = buffer()->GetBulk(this, &out_ptr, &out_count, Count());
  if (!ret) {
    return false;
  }
  // update count.
  DCHECK(Count() <= out_count);
  set_count(out_count + 1);

  if (bulk_ptr) {
    *bulk_ptr = out_ptr;
  }
  if (bulk_count) {
    *bulk_count = out_count;
  }

  return true;
}


template<typename BulkPtrType>
bool SequencedBulkQueue<BulkPtrType>::Reader::WaitGetBulk(BulkPtrType* bulk_ptr,
                                                           int64* bulk_count) {
  bool ret = WaitForReady();

  if (!ret) {
    return false;
  }
  if (bulk_ptr) {
    *bulk_ptr = temp_ptr_;
  }
  if (bulk_count) {
    *bulk_count = temp_count_;
  }
  return true;
}

template<typename BulkPtrType>
bool SequencedBulkQueue<BulkPtrType>::Reader::WaitTimeoutGetBulk(BulkPtrType* bulk_ptr,
                                                                  int64* bulk_count,
                                                                  base::TimeDelta delta) {
  bool ret = WaitTimeoutForReady(delta);

  if (!ret) {
    return false;
  }

  if (bulk_ptr) {
    *bulk_ptr = temp_ptr_;
  }
  if (bulk_count) {
    *bulk_count = temp_count_;
  }
  return true;
}

template<typename BulkPtrType>
bool SequencedBulkQueue<BulkPtrType>::CheckReady(int64 count, const base::Closure& ready) {
  base::AutoLock lock(lock_);
  if (count > CountNoLock()) {
    return false;
  }
  if (buffer_queue_.size() == 0) {
    return false;
  }
  BulkPieceType last_bulk = buffer_queue_.back();
  bool ret = last_bulk.count >= count;
  if (ret) {
    ready.Run();
  }
  return ret;
}
