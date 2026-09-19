#include "gtest/gtest.h"
#include "idf_fake.h"
#include "roo_time_ds3231.h"

#if defined(ARDUINO)
#error Native RTC test must not depend on Arduino.
#endif
namespace roo_time {
// Verifies native RTC initialization, register payloads, and transport
// recovery.
TEST(IdfClock, InitializationReadWriteAndRecovery) {
  i2c_test::state() = i2c_test::State();
  const WallTime expected =
      DateTime(2024, 6, 15, 12, 34, 56, 0, timezone::UTC).wallTime();
  roo_io::I2cMasterBusHandle bus;
  Ds3231Clock rtc(bus, timezone::UTC, Micros(0));
  EXPECT_FALSE(rtc.now().isSet());
  EXPECT_FALSE(rtc.init());
  ASSERT_TRUE(bus.init(18, 19));
  ASSERT_TRUE(rtc.init());
  EXPECT_TRUE(rtc.init());
  EXPECT_EQ(expected, rtc.now());
  EXPECT_EQ(0x68, i2c_test::state().address);
  EXPECT_EQ((std::vector<uint8_t>{0}), i2c_test::state().sent);
  EXPECT_EQ(1, i2c_test::state().combined);
  ASSERT_TRUE(rtc.set(expected));
  EXPECT_EQ((std::vector<uint8_t>{0, 0x56, 0x34, 0x12, 6, 0x15, 0x06, 0x24}),
            i2c_test::state().sent);
  i2c_test::state().transfer_result = ESP_ERR_TIMEOUT;
  EXPECT_FALSE(rtc.now().isSet());
  EXPECT_FALSE(rtc.set(expected));
  i2c_test::state().transfer_result = ESP_OK;
  EXPECT_EQ(expected, rtc.now());
}
// Verifies a failed native write invalidates an otherwise valid cached reading.
TEST(IdfClock, FailedWriteInvalidatesCachedTime) {
  i2c_test::state() = i2c_test::State();
  i2c_test::state().available = true;
  Ds3231Clock rtc;
  ASSERT_TRUE(rtc.init());
  ASSERT_TRUE(rtc.now().isSet());
  i2c_test::state().transfer_result = ESP_FAIL;
  EXPECT_TRUE(rtc.now().isSet());
  EXPECT_FALSE(rtc.set(DateTime(2024, 6, 15, timezone::UTC).wallTime()));
  EXPECT_FALSE(rtc.now().isSet());
  i2c_test::state().transfer_result = ESP_OK;
  EXPECT_TRUE(rtc.now().isSet());
}
}  // namespace roo_time
