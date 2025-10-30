#include "roo_time_ds3231.h"

#include "gtest/gtest.h"
#include "roo_testing/devices/clock/ds3231/ds3231.h"
#include "roo_testing/microcontrollers/esp32/fake_esp32.h"

namespace roo_time {

struct Emulator {
  FakeDs3231 rtc;
  Emulator() { FakeEsp32().attachI2cDevice(rtc, 18, 19); }
} emulator;

TEST(Ds3231Clock, Basic) {
  Wire.begin(18, 19);
  Ds3231Clock rtc;
  WallTime start = DateTime(2020, 1, 1, 0, 0, 0, 0, timezone::UTC).wallTime();
  rtc.set(start);
  WallTime t1 = rtc.now();
  EXPECT_EQ(t1, start);
  delay(3100);
  WallTime t2 = rtc.now();
  EXPECT_EQ(t2, start + Seconds(3));
}

}  // namespace roo_time
