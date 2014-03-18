#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "base/bind.h"

#include "device/memory_frame_pool.h"

using namespace base;

TEST(MemoryFramePoolTest, TestAlloc) {
  scoped_refptr<MemoryFramePool> pool = new MemoryFramePool(100, 3);
  uint8* frame1 = pool->Alloc();
  EXPECT_TRUE(frame1 != NULL);
  uint8* frame2 = pool->Alloc();
  EXPECT_TRUE(frame2 != NULL);
  EXPECT_TRUE(pool->Left() == 1);
  uint8* frame3 = pool->Alloc();
  EXPECT_TRUE(frame3 != NULL);
  uint8* frame4 = pool->Alloc();
  EXPECT_TRUE(frame4 == NULL);
  pool->Free(frame3);
  frame4 = pool->Alloc();
  EXPECT_TRUE(frame4 != NULL);
  pool->Free(frame1);
  pool->Free(frame2);
  pool->Free(frame4);
}

namespace {
class FreeFrameMock {
public:
  FreeFrameMock() {}
  virtual ~FreeFrameMock() {}
  MOCK_METHOD0(OnFree, void(void));
private:

};
}

TEST(MemoryFramePoolTest, TestFreeCallback) {
  scoped_refptr<MemoryFramePool> pool = new MemoryFramePool(100, 1);
  FreeFrameMock free_call;
  pool->set_have_free_callback(Bind(&FreeFrameMock::OnFree, Unretained(&free_call)));
  EXPECT_CALL(free_call, OnFree()).Times(0);
  uint8* frame = pool->Alloc();
  EXPECT_CALL(free_call, OnFree()).Times(1);
  pool->Free(frame);
  frame = pool->Alloc();
  EXPECT_CALL(free_call, OnFree()).Times(1);
  pool->Free(frame);
}