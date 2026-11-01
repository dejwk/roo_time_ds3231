#include <type_traits>

#include "gtest/gtest.h"
#include "roo_time_ds3231.h"

// Verifies legacy include/type, constructors, and WallTimeClock integration.
TEST(Ds3231Compatibility, PublicApiForwardsToRooRtc) {
  static_assert(
      std::is_same<roo_time::Ds3231Clock, roo_rtc::Ds3231Clock>::value,
      "Legacy type must forward to the maintained driver");
  roo_time::Ds3231Clock default_clock;
  roo_time::Ds3231Clock offset_clock(roo_time::UtcOffset(roo_time::Hours(2)),
                                     roo_time::Seconds(5));
  roo_time::Ds3231Clock bus_clock(Wire, roo_time::timezone::UTC,
                                  roo_time::Micros(0));
  roo_time::WallTimeClock& base = bus_clock;
  (void)base;
  EXPECT_FALSE(default_clock.set(roo_time::WallTime::Unset()));
  EXPECT_FALSE(offset_clock.set(
      roo_time::DateTime(2100, 1, 1, roo_time::timezone::UTC).wallTime()));
}
