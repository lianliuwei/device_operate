#include "testing/gtest/include/gtest/gtest.h"
#include "testing/gmock/include/gmock/gmock.h"

#include "base/bind.h"

#include "device/memory_usage_pool.h"

using namespace base;
using testing::_;

TEST(MemoryUsagePoolTest, TestAlloc) {
  scoped_refptr<MemoryUsagePool> pool = new MemoryUsagePool(100);
  uint8* buffer1 = pool->Alloc(10);
  EXPECT_TRUE(buffer1 != NULL);
  uint8* buffer2 = pool->Alloc(30);
  EXPECT_TRUE(buffer2 != NULL);
  uint8* buffer3 = pool->Alloc(61);
  EXPECT_TRUE(buffer3 == NULL);
  pool->Free(buffer1);
  buffer3 = pool->Alloc(61);
  EXPECT_TRUE(buffer3 != NULL);
  pool->Free(buffer2);
  pool->Free(buffer3);
}

namespace {
class NeedFreeMock {
public:
  NeedFreeMock() {}
  virtual ~NeedFreeMock() {}
  MOCK_METHOD1(OnFree, void(int));

};
}

TEST(MemoryUsagePoolTest, TestFreeCallback) {
  scoped_refptr<MemoryUsagePool> pool = new MemoryUsagePool(100);
  NeedFreeMock need_free;
  
  EXPECT_CALL(need_free, OnFree(100)).Times(1);
  pool->SetFreeCallback(Bind(&NeedFreeMock::OnFree, Unretained(&need_free)), 50);

  EXPECT_CALL(need_free, OnFree(51)).Times(1);
  uint8* buffer1 = pool->Alloc(49);
  pool->SetFreeCallback(Bind(&NeedFreeMock::OnFree, Unretained(&need_free)), 50);
  pool->Free(buffer1);
  buffer1 = pool->Alloc(10);
  pool->Free(buffer1);
  buffer1 = pool->Alloc(41);
  pool->Free(buffer1);

  EXPECT_CALL(need_free, OnFree(_)).Times(0);
  buffer1 = pool->Alloc(51);
  pool->SetFreeCallback(Bind(&NeedFreeMock::OnFree, Unretained(&need_free)), 50);
  EXPECT_CALL(need_free, OnFree(100)).Times(1);
  pool->Free(buffer1);
}