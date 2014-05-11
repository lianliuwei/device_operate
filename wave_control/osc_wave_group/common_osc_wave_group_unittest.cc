#include "testing/gtest/include/gtest/gtest.h"

#include "wave_control/osc_wave_group/common_osc_wave_group.h"
#include "wave_control/examples/test_osc_wave.h"


TEST(CommonOscWaveGroupTest, normal) {
  CommonOscWaveGroup wave_group;
  std::vector<TestOscWave*> waves = CreateWaves();
  for (size_t i = 0; i < waves.size(); ++i) {
    wave_group.AddOscWave(waves[i]);
  }
  EXPECT_EQ(wave_group.trigger_count(), 1);
  EXPECT_EQ(wave_group.horizontal_count(), 1);
  EXPECT_EQ(wave_group.vertical_count(), 3);

  TestOscWave* remove_wave = waves[0];
  wave_group.RemoveOscWave(remove_wave);

  EXPECT_EQ(wave_group.trigger_count(), 1);
  EXPECT_EQ(wave_group.horizontal_count(), 1);
  EXPECT_EQ(wave_group.vertical_count(), 2);

  EXPECT_TRUE(wave_group.trigger_at(0)->osc_wave() != remove_wave);
  EXPECT_TRUE(wave_group.horizontal_at(0)->osc_wave() != remove_wave);
}