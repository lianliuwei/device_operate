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
  \"CANL.Range\" : 3, \
  \"CANL.Offset\" : 0.0, \
  \"CANL.Coupling\" : 1, \
  \"CANDIFF.Range\" : 3, \
  \"CANDIFF.Offset\" : 0.0, \
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

 OscRawDataHandle diff_osc_raw_data = new OscRawData(DT_CS1202, 
      new OscRawDataDeviceConfig(config, true));
  
  scoped_refptr<ChnlCalcResult> diff_result = new ChnlCalcResult(diff_osc_raw_data);
  container.UpdateResult(diff_result);
  EXPECT_EQ(1, container.ChnlCount());
}

