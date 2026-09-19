#include "roo_time_ds3231.h"

#include "gtest/gtest.h"
#include "roo_testing/devices/clock/ds3231/ds3231.h"
#include "roo_testing/microcontrollers/esp32/fake_esp32.h"

namespace roo_time {

/// Keeps the attached RTC alive for the emulator process.
struct Emulator {
  FakeDs3231 rtc;
  /// Connects the RTC to the test bus pins.
  Emulator() { FakeEsp32().attachI2cDevice(rtc, 18, 19); }
} emulator;

// Verifies both backends set/read an emulated RTC and extrapolate cached time.
TEST(Ds3231Clock, Basic) {
  roo_io::I2cMasterBusHandle bus;
  ASSERT_TRUE(bus.init(18, 19));
  Ds3231Clock rtc(bus);
  ASSERT_TRUE(rtc.init());
  WallTime start = DateTime(2020, 1, 1, 0, 0, 0, 0, timezone::UTC).wallTime();
  ASSERT_TRUE(rtc.set(start));
  WallTime t1 = rtc.now();
  EXPECT_EQ(t1, start);
  Ds3231Clock reader(bus, timezone::UTC, Micros(0));
  ASSERT_TRUE(reader.init());
  EXPECT_EQ(start, reader.now());
  Delay(Millis(3100));
  WallTime t2 = rtc.now();
  EXPECT_EQ(t2, start + Seconds(3));
}

}  // namespace roo_time
