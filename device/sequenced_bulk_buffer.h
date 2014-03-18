#pragma once

template<typename BulkPtrType>
struct BulkPiece {
  BulkPtrType bulk;
  int64 count;
};

// TODO need restart push bulk after bulk full.
// this add in memory pool callback.
// void ReStartPush();


// use ref_count ptr type
template<typename BulkPtrType>
class SequencedBulkBuffer : public base::RefCountedThreadSafe<SequencedBulkBuffer> {
public:
  typedef BulkPiece<BulkPtrType> BulkPieceType;

  // sync reader method use in another thread.
  class Reader {
  public:
    Reader(SequencedBulkBuffer* seq);
    ~Reader();

    typedef base::Callback<void(void)> BufferHaveDataCallback;

    int64 Count() { return count_; }

    // return true if bluck get, bluck_count >= last_count
    // if bluck_count > last_count mean data lost by overwrite.
    bool GetBluk(BulkPtrType* bluck_ptr, int64* bluck_count);

    void Skip(int64 n) { count_ += n; }

    // sync wait
    void WaitForReady();
    void WaitTimeoutForReady(base::TimeDelta delta);

    // async
    void CallbackOnReady();

    // can not change when in async wait. 
    // callback no nest in OnBufferReady()
    void set_have_data_callback(BufferHaveDataCallback have_data) {
      have_data_ = have_data;
    }

    bool IsQuit() const { return quiting_; }

  private:
    // for sync
    void OnWaitReady(int64 count, bool quit);

    // for async
    void OnBufferReady(base::SequencedTaskRunner* runner, int64 count, bool quit);

    BufferHaveDataCallback have_data_;

    int64 count_;
    scoped_refptr<SequencedBulkBuffer> buffer_;
    WaitableEvent event_;
    bool quiting_;

    DISALLOW_COPY_AND_ASSIGN(Reader);
  };


  void Quit();

  bool GetBluk(Reader* reader, BulkPtrType* bluck_ptr, int64* bluck_count, int64 count);
  
  void RegisterReader(Reader* reader, int64 start_count);
  void UnRegisterReader(Reader* reader);

  // void OnBufferReady(int64 count, bool quit);
  typedef base::Callback<void(int64, bool)> BufferReadyCallback;

  // one time callback.
  // if callback match, call immediate.
  void SetReadyCallback(void* id, int64 count, BufferReadyCallback ready_callback);
  void CancelCallback(void* id);

  // write method
  // this will inc bulk count.
  void PushBluk(BulkPtrType bulk);

  int bulk_num() { buffer_queue_.size(); }

  int64 Count() const { return count_; }

  // recycle one is for over-write mode, recycle no be read buffer first
  // and recycle between reader buffer then,
  // if all is occupy, means need more buffer, or need process faster.
  bool RecycleOne();

private:
  // these method no lock
  // 
  void FireCallback(int64 reach_count);
  void UpdateReaderCount(Reader* reader, count);
  // Release the buffer the all reader read.
  void MayReleaseBuffer();
  // find bulk which have the most small of the bigger or equal count 
  bool FindBulk(BulkPtrType* bluck_ptr, int64* bluck_count, int64 count);

  base::Lock lock_;
  
  struct ReadyCallbackRecord {
    BufferReadyCallback callback;
    int64 count;
  }

  // each reader progress
  typedef std::map<Reader*, int64> ReaderCountMap;
  ReaderCountMap count_map_;

  // register callback when count reach
  typedef std::map<void*, ReadyCallbackRecord> ReadyCallbackMap;
  ReadyCallbackMap callback_map_;

  // reader current_count_ range
  int64 reader_front_count_;
  int64 reader_back_count_;

  bool quiting_;
  bool over_write_;

  // total count
  int64 count_;

  std::list<BulkPiece> buffer_queue_;


  DISALLOW_COPY_AND_ASSIGN(SequencedBulkBuffer);
};


template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::RegisterReader(Reader* reader, 
                                                      int64 start_count) {
 {
  base::AutoLock lock(lock_);
  
  ReaderCountMap::iterator it = std::find(count_map_.begin(), count_map_.end(), reader);
  DCHECK(it == count_map_.end());
  // start_count mean no read yet
  int count = start_count - 1;
  count_map_.insert(std::make_pair(reader, count));

  if (count > reader_back_count_) {
    reader_back_count_ = count;
  }
  int64 min_value =count;
  for (ReaderCountMap::iterator it = count_map_.begin();
       it != count_map_.end();
       ++it) {
    if (min_value > it->second) {
      min_value = it->second;
    }
  }
  reader_front_count_ = min_value;
  }

}

template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::UnRegisterReader(Reader* reader) {
  {
  base::AutoLock lock(lock_);

  ReaderCountMap::iterator it = std::find(count_map_.begin(), count_map_.end(), reader);
  DCHECK(it != count_map_.end());
  count_map_.erase(it);
  }
}


template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::UpdateReaderCount(Reader* reader, count) {
  {
  base::AutoLock lock(lock_);

  ReaderCountMap::iterator it = std::find(count_map_.begin(), count_map_.end(), reader);
  DCHECK(it != count_map_.end());
  if (it->second == count) {
    return;
  }
  it->second = count;
  if (count > reader_back_count_) {
    reader_back_count_ = count;
  }
  int64 min_value =count;
  for (ReaderCountMap::iterator it = count_map_.begin();
       it != count_map_.end();
       ++it) {
    if (min_value > it->second) {
      min_value = it->second;
    }
  }
  reader_front_count_ = min_value;
  }
}

template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::MayReleaseBuffer() {
  std::list<BulkPiece>::iterator it = buffer_queue_.begin();
  while(it != buffer_queue_.end()) {
    if (it->count <= reader_front_count_) {
      it->bulk = NULL;
      std::list<BulkPiece>::iterator remove_it = it;
      ++it;
      buffer_queue_.erase(it);
    } else {
      break;
    }
  }
}

template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::Quit() {
  {
  base::AutoLock lock(lock_);

  DCHECK(quiting_ == false);
  quiting_ = true;

  for (ReadyCallbackMap::iterator it = callback_map_.begin();
       it != callback_map_.end();
       ++it) {
    it->second.callback.Run(count_, true);
  }
  }
}


template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::SetReadyCallback(void* id, int64 count, 
                                                        BufferReadyCallback ready_callback) {
  {
  base::AutoLock lock(lock_);

  ReadyCallbackMap::iterator it = std::find(callback_map_.begin(), callback_map_.end(), id);
  DCHECK(it == callback_map_.end());
  ReadyCallbackRecord record = { ready_callback, count, };
  callback_map_.insert(std::make_pair(id, record));
  }
  }
}


template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::CancelCallback(void* id) {
  {
  base::AutoLock lock(lock_);

  ReadyCallbackMap::iterator it = std::find(callback_map_.begin(), callback_map_.end(), id);
  // callback may just fire, before Cancelcallback
  if (it == callback_map_.end()) {
    return;
  }
  callback_map_.erase(it);
  }
  }
}

template<typename BulkPtrType>
void SequencedBulkBuffer<BulkPtrType>::FireCallback(int64 reach_count) {
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

template<typename BulkPtrType>
bool SequencedBulkBuffer<BulkPtrType>::GetBluk(Reader* reader, 
                                               BulkPtrType* bluck_ptr, int64* bluck_count, 
                                               int64 count) {
  {
  base::AutoLock lock(lock_);

  BulkPieceType out_ptr;
  int64 out_count;
  bool ret = FindBulk(out_ptr, out_count, count);
  if (ret) {
    // auto release no used buffer
    UpdateReaderCount(reader, count);
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
void SequencedBulkBuffer<BulkPtrType>::PushBluk(BulkPtrType bulk) {
  {
  base::AutoLock lock(lock_);

  buffer_queue_.push_back(BulkPiece(bulk, count_));
  FireCallback(count_);
  ++count_;
  }
}

template<typename BulkPtrType>
bool SequencedBulkBuffer<BulkPtrType>::FindBulk(BulkPtrType* bluck_ptr, 
                                                int64* bluck_count, 
                                                int64 count) {
  // bulk no in queue now
  if (count_ <= count) {
    return false;
  }
  for (std::list<BulkPiece>::iterator it = buffer_queue_.begin();
       it != buffer_queue_.end();
       ++it) {
    if (it->count >= count) {
      *bluck_ptr = it->bulk;
      *bluck_count = it->count;
      return true;
    }
  }
  return true;
}

template<typename BulkPtrType>
void SequencedBulkBuffer::Reader<BulkPtrType>::Quit() {
  // callback.run(count, true);
}

template<typename BulkPtrType>
void SequencedBulkBuffer::Reader<BulkPtrType>::WaitForReady() {
  CHECK(!IsQuit());

  buffer_->SetReadyCallback(this, 
      base::Bind(&SequencedBulkBuffer::Reader::OnWaitReady, 
                 base::Unretained(this)));
  event_.Wait();
  event_.Reset();
}

template<typename BulkPtrType>
bool SequencedBulkBuffer::Reader<BulkPtrType>::WaitTimeoutForReady(
    const TimeDelta& delta) {
  CHECK(!IsQuit());

  buffer_->SetReadyCallback(this, 
      base::Bind(&SequencedBulkBuffer::Reader::OnWaitReady, 
                 base::Unretained(this)));
  bool ret = event_.TimedWait(delta);
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


template<typename BulkPtrType>
void SequencedBulkBuffer::Reader<BulkPtrType>::OnWaitReady(int64 count, bool quit) {
  quiting_ = quit;
  event_.Signal();
}

template<typename BulkPtrType>
void SequencedBulkBuffer::Reader<BulkPtrType>::CallbackOnReady() {
  CHECK(!IsQuit());

  base::SequencedTaskRunner* runner = MessageLoopProxy::current();
  DCHECK(runner) << "need runner for async OnBufferReady call";
  buffer_->SetReadyCallback(this, 
      base::Bind(&SequencedBulkBuffer::Reader::OnBufferReady, 
                 base::Unretained(this),
                 runner));
}


template<typename BulkPtrType>
void SequencedBulkBuffer::Reader<BulkPtrType>::OnBufferReady(
    base::SequencedTaskRunner* runner, int64 count, bool quit) {
  quiting_ = quit;
 
  if (!have_data_.is_null()) {
    runner->PostTask(FROM_HERE, have_data_);
  }
}


template<typename BulkPtrType>
bool SequencedBulkBuffer::Reader<BulkPtrType>::GetBluk(BulkPtrType* bluck_ptr, 
                                                       int64* bluck_count) {
  BulkPieceType out_ptr;
  int64 out_count;
  bool ret = buffer_->GetBluk(this, &out_ptr, &out_count);
  if (!ret) {
    return false;
  }
  // update count.
  DCHECK(count_ <= out_count);
  count_ = out_count + 1;

  if (bluck_ptr) {
    *bluck_ptr = out_ptr;
  }
  if (bluck_count) {
    *bluck_count = out_count;
  }

  return true;
}

