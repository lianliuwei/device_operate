#include "testing/gtest/include/gtest/gtest.h"

#include "canscope/chnl/canscope_chnl_container.h"
#include "canscope/device/config_util.h"

using namespace canscope;

namespace {
static const char kOscConfig [] =  {" \
{ \
  \"CANH.Range\" : 3, \
  \"CANH.Offset\" : 0.0, \
  \"CANH.Coupling\" : 1, \
  \"CANL.Range\" : 4, \
  \"CANL.Offset\" : 1.0, \
  \"CANL.Coupling\" : 1, \
  \"CANDIFF.Range\" : 5, \
  \"CANDIFF.Offset\" : 2.0, \
  \"DiffCtrl\" : 0, \
  \"Time.Base\" : 4, \
  \"Time.Offset\" : 0.0, \
  \"Trigger.AutoTime\" : 100.0, \
  \"Trigger.Source\" : 2, \
  \"Trigger.Type\" : 0, \
  \"Trigger.Mode\" : 0, \
  \"Trigger.Sens\" : 0, \
  \"Trigger.Compare\" : 0, \
  \"Trigger.Volt\" : 0.0, \
  \"TimeParam\" : 1.0 \
} \
"};

}
TEST(CANScopeChnlContainerTest, ChnlTest) {
  ConfigManager::Config config = { 0,  GetConfig(kOscConfig) };
  OscRawDataHandle osc_raw_data = new OscRawData(DT_CS1202, 
      new OscRawDataDeviceConfig(config, false));
  
  scoped_refptr<ChnlCalcResult> result = new ChnlCalcResult(osc_raw_data);
  
  CANScopeChnlContainer container(result, NULL);
  EXPECT_EQ(3, container.ChnlCount());

  ::Chnl* can_h = container.GetChnlAt(0);
  ::Chnl* can_l = container.GetChnlAt(1);
  ::Chnl* can_diff = container.GetChnlAt(2);
  EXPECT_EQ(CHNL_CAN_H, can_h->ChnlType());
  EXPECT_EQ(CHNL_CAN_L, can_l->ChnlType());
  EXPECT_EQ(CHNL_CAN_DIFF, can_diff->ChnlType());
  EXPECT_EQ(can_h, container.GetChnlByType(CHNL_CAN_H));
  EXPECT_EQ(can_l, container.GetChnlByType(CHNL_CAN_L));
  EXPECT_EQ(can_diff, container.GetChnlByType(CHNL_CAN_DIFF));
  EXPECT_EQ(can_h->VRange(), 3);
  EXPECT_DOUBLE_EQ(can_h->VRangeValue(), 8.0);
  EXPECT_DOUBLE_EQ(can_h->VOffset(), 0.0);
  EXPECT_EQ(can_l->VRange(), 4);
  EXPECT_DOUBLE_EQ(can_l->VRangeValue(), 20);
  EXPECT_DOUBLE_EQ(can_l->VOffset(), 1.0);
  EXPECT_EQ(can_diff->VRange(), 5);
  EXPECT_DOUBLE_EQ(can_diff->VRangeValue(), 50);
  EXPECT_DOUBLE_EQ(can_diff->VOffset(), 2.0);


  OscRawDataHandle diff_osc_raw_data = new OscRawData(DT_CS1202, 
      new OscRawDataDeviceConfig(config, true));
  
  scoped_refptr<ChnlCalcResult> diff_result = new ChnlCalcResult(diff_osc_raw_data);
  container.UpdateResult(diff_result);
  EXPECT_EQ(1, container.ChnlCount());
  can_diff = container.GetChnlAt(0);
  EXPECT_EQ(CHNL_CAN_DIFF, can_diff->ChnlType());
  EXPECT_EQ(can_diff, container.GetChnlByType(CHNL_CAN_DIFF));
  EXPECT_EQ(can_diff->VRange(), 3);
  EXPECT_DOUBLE_EQ(can_diff->VRangeValue(), 8.0);
  EXPECT_DOUBLE_EQ(can_diff->VOffset(), 0.0);

  container.UpdateResult(result);
  EXPECT_EQ(3, container.ChnlCount());

  can_h = container.GetChnlAt(0);
  can_l = container.GetChnlAt(1);
  can_diff = container.GetChnlAt(2);
  EXPECT_EQ(CHNL_CAN_H, can_h->ChnlType());
  EXPECT_EQ(CHNL_CAN_L, can_l->ChnlType());
  EXPECT_EQ(CHNL_CAN_DIFF, can_diff->ChnlType());
  EXPECT_EQ(can_h, container.GetChnlByType(CHNL_CAN_H));
  EXPECT_EQ(can_l, container.GetChnlByType(CHNL_CAN_L));
  EXPECT_EQ(can_diff, container.GetChnlByType(CHNL_CAN_DIFF));
  EXPECT_EQ(can_h->VRange(), 3);
  EXPECT_DOUBLE_EQ(can_h->VRangeValue(), 8.0);
  EXPECT_DOUBLE_EQ(can_h->VOffset(), 0.0);
  EXPECT_EQ(can_l->VRange(), 4);
  EXPECT_DOUBLE_EQ(can_l->VRangeValue(), 20);
  EXPECT_DOUBLE_EQ(can_l->VOffset(), 1.0);
  EXPECT_EQ(can_diff->VRange(), 5);
  EXPECT_DOUBLE_EQ(can_diff->VRangeValue(), 50);
  EXPECT_DOUBLE_EQ(can_diff->VOffset(), 2.0);
}

