#include "testing/gtest/include/gtest/gtest.h"

#include "base/basictypes.h"

#include "canscope/device/register/scope_ctrl_register.h"

using namespace canscope;

TEST(ScopeCtrlRegisterTest, SetContentAndCheckProperty) {
  uint8 scope_ctrl_content[0x04] 
     = {0x01, 0x00, 0x00, 0x00,
  };
  ScopeCtrlRegister scope_ctrl;
  memcpy(scope_ctrl.memory.buffer(), scope_ctrl_content, 
    arraysize(scope_ctrl_content));
  
  EXPECT_EQ(true, scope_ctrl.scope_ctrl.value());
}