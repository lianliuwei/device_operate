#include "testing/gtest/include/gtest/gtest.h"

#include "base/threading/thread.h"
#include "base/callback.h"
#include "base/bind.h"
#include "base/memory/scoped_vector.h"

#include "device/sequenced_bulk_buffer.h"

using namespace base;

namespace {
struct TestBulk : public base::RefCountedThreadSafe<TestBulk> {
public:
  int i;
};

typedef scoped_refptr<TestBulk> TestBulkHandle;

typedef SequencedBulkBuffer<TestBulkHandle> TestBulkQueue;
}

TEST(SequencedBulkBufferTest, NoCache) {
  scoped_refptr<TestBulkQueue> bulk_queue = new TestBulkQueue(false, false);
  for (int i = 0; i < 100; ++i) {
    TestBulkHandle test_bulk = new TestBulk();
    test_bulk->i = i;
    bulk_queue->PushBluk(test_bulk);
  }
  EXPECT_EQ(0, bulk_queue->bulk_num());
}

TEST(SequencedBulkBufferTest, OneReaderCacheAll) {
  scoped_refptr<TestBulkQueue> bulk_queue = new TestBulkQueue(false, false);
  TestBulkQueue::Reader queue_reader(bulk_queue);
  for (int i = 0; i < 100; ++i) {
    TestBulkHandle test_bulk = new TestBulk();
    test_bulk->i = i;
    bulk_queue->PushBluk(test_bulk);
  }
  EXPECT_EQ(100, bulk_queue->bulk_num());
}

TEST(SequencedBulkBufferTest, OneReaderAndSkip) {
  scoped_refptr<TestBulkQueue> bulk_queue = new TestBulkQueue(false, false);
  TestBulkQueue::Reader queue_reader(bulk_queue);
  for (int i = 0; i < 100; ++i) {
    TestBulkHandle test_bulk = new TestBulk();
    test_bulk->i = i;
    bulk_queue->PushBluk(test_bulk);
  }
  for (int i = 0; i < 10; ++i) {
    TestBulkHandle read_bulk;
    int64 read_count;
    EXPECT_EQ(queue_reader.Count(), i*3);
    queue_reader.GetBluk(&read_bulk, &read_count);
    queue_reader.Skip(2);
    EXPECT_EQ(read_count, i*3);
    EXPECT_EQ(read_bulk->i, i*3);
  }
  // last skip no pass to queue
  EXPECT_EQ(100 - 10*3 + 2, bulk_queue->bulk_num());
}

TEST(SequencedBulkBufferTest, OneReaderCacheLeft) {
  scoped_refptr<TestBulkQueue> bulk_queue = new TestBulkQueue(false, false);
  TestBulkQueue::Reader queue_reader(bulk_queue);
  for (int i = 0; i < 100; ++i) {
    TestBulkHandle test_bulk = new TestBulk();
    test_bulk->i = i;
    bulk_queue->PushBluk(test_bulk);
  }
  for (int i = 0; i < 50; ++i) {
    TestBulkHandle read_bulk;
    int64 read_count;
    EXPECT_EQ(queue_reader.Count(), i);
    queue_reader.GetBluk(&read_bulk, &read_count);
    EXPECT_EQ(read_count, i);
    EXPECT_EQ(read_bulk->i, i);
  }
  EXPECT_EQ(100 - 50, bulk_queue->bulk_num());
}

TEST(SequencedBulkBufferTest, TwoReaderCacheLeft) {
  scoped_refptr<TestBulkQueue> bulk_queue = new TestBulkQueue(false, false);
  TestBulkQueue::Reader queue_reader1(bulk_queue);
  TestBulkQueue::Reader queue_reader2(bulk_queue);
  for (int i = 0; i < 100; ++i) {
    TestBulkHandle test_bulk = new TestBulk();
    test_bulk->i = i;
    bulk_queue->PushBluk(test_bulk);
  }
  for (int i = 0; i < 50; ++i) {
    TestBulkHandle read_bulk;
    int64 read_count;
    EXPECT_EQ(queue_reader1.Count(), i);
    queue_reader1.GetBluk(&read_bulk, &read_count);
    EXPECT_EQ(read_count, i);
    EXPECT_EQ(read_bulk->i, i);
  }
  for (int i = 0; i < 50; ++i) {
    TestBulkHandle read_bulk;
    int64 read_count;
    EXPECT_EQ(queue_reader2.Count(), i);
    queue_reader2.GetBluk(&read_bulk, &read_count);
    EXPECT_EQ(read_count, i);
    EXPECT_EQ(read_bulk->i, i);
  }

  EXPECT_EQ(100 - 50, bulk_queue->bulk_num());

  for (int i = 50; i < 70; ++i) {
    TestBulkHandle read_bulk;
    int64 read_count;
    EXPECT_EQ(queue_reader1.Count(), i);
    queue_reader1.GetBluk(&read_bulk, &read_count);
    EXPECT_EQ(read_count, i);
    EXPECT_EQ(read_bulk->i, i);
    EXPECT_EQ(queue_reader2.Count(), i);
    queue_reader2.GetBluk(&read_bulk, &read_count);
    EXPECT_EQ(read_count, i);
    EXPECT_EQ(read_bulk->i, i);
  }
  EXPECT_EQ(100 - 70, bulk_queue->bulk_num());
}


namespace {
enum ThreadReaderTestType {
  kFullSpeed,
  kWaitTimeout,
};

class ThreadReader {
public:
  ThreadReader(ThreadReaderTestType test_type, 
               scoped_refptr<TestBulkQueue> bulk_queue, 
               int num, 
               int thread_num)
      : bulk_queue_(bulk_queue) 
      , thread_num_(thread_num)
      , finish_thread_num_(0)
      , event_(true, false) {
   switch (test_type) {
   case kFullSpeed: FullSpeed(num); break;
   case kWaitTimeout: WaitTimeout(num); break;
   default: NOTREACHED();
   };
  }

  ~ThreadReader() {}

  void WaitForFinish() {
    event_.Wait();
  }

  void ReaderFinish() {
    bool notify = false; 
    {
    AutoLock lock(lock_);
    ++finish_thread_num_;
    if (finish_thread_num_ == thread_num_) {
      notify = true;
    }
    }
    if (notify) {
      event_.Signal();
    }
  }

  void WaitTimeout(int bulk_num) {
    event_.Reset();
    for (int i = 0; i < thread_num_; ++i) {
      Thread* temp = new Thread("BulkReader");
      threads_.push_back(temp);
      temp->Start();
      temp->message_loop()->PostTask(FROM_HERE, 
        Bind(&ThreadReader::WaitTimeoutReader, Unretained(this), bulk_num));
    }
  }

  void WaitTimeoutReader(int num) {
    TestBulkQueue::Reader queue_reader(bulk_queue_);
    for (int i = 0; i < num; ++i) {
      TestBulkHandle read_bulk;
      int64 read_count;
      EXPECT_EQ(queue_reader.Count(), i);
      bool ret = false;
      while (!ret) {
       ret = queue_reader.WaitTimeoutForReady(TimeDelta::FromMilliseconds(100));
      }
      ret = queue_reader.GetBluk(&read_bulk, &read_count);
      EXPECT_TRUE(ret);
      EXPECT_EQ(read_count, i);
      EXPECT_EQ(read_bulk->i, i);
    }
    ReaderFinish();
  }

  void FullSpeed(int bulk_num) {
    event_.Reset();
    for (int i = 0; i < thread_num_; ++i) {
      Thread* temp = new Thread("BulkReader");
      threads_.push_back(temp);
      temp->Start();
      temp->message_loop()->PostTask(FROM_HERE, 
        Bind(&ThreadReader::FullSpeedReader, Unretained(this), bulk_num));
    }
  }

  void FullSpeedReader(int num) {
    TestBulkQueue::Reader queue_reader(bulk_queue_);
    for (int i = 0; i < num; ++i) {
      TestBulkHandle read_bulk;
      int64 read_count;
      EXPECT_EQ(queue_reader.Count(), i);
      queue_reader.WaitForReady();
      bool ret = queue_reader.GetBluk(&read_bulk, &read_count);
      EXPECT_TRUE(ret);
      EXPECT_EQ(read_count, i);
      EXPECT_EQ(read_bulk->i, i);
    }
    ReaderFinish();
  }

private:
  scoped_refptr<TestBulkQueue> bulk_queue_;
  base::WaitableEvent event_;
  base::Lock lock_;
  int thread_num_;
  int finish_thread_num_;

  ScopedVector<Thread> threads_;
  DISALLOW_COPY_AND_ASSIGN(ThreadReader);
};
}

TEST(SequencedBulkBufferTest, FullSpeedReader) {
  scoped_refptr<TestBulkQueue> bulk_queue = new TestBulkQueue(false, true);
  ThreadReader thread_reader(kFullSpeed, bulk_queue, 1000, 10);

  for (int i = 0; i < 1020; ++i) {
    TestBulkHandle test_bulk = new TestBulk();
    test_bulk->i = i;
    bulk_queue->PushBluk(test_bulk);
  }
  thread_reader.WaitForFinish();
  EXPECT_EQ(20, bulk_queue->bulk_num());
}

TEST(SequencedBulkBufferTest, WaitTimeout) {
  scoped_refptr<TestBulkQueue> bulk_queue = new TestBulkQueue(false, true);
  ThreadReader thread_reader(kWaitTimeout, bulk_queue, 10, 10);

  for (int i = 0; i < 12; ++i) {
    TestBulkHandle test_bulk = new TestBulk();
    test_bulk->i = i;
    bulk_queue->PushBluk(test_bulk);
    base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(110));
  }
  thread_reader.WaitForFinish();
  EXPECT_EQ(2, bulk_queue->bulk_num());
}
