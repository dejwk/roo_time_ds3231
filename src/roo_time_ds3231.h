#pragma once

/// Compatibility header for the DS3231 driver now maintained in roo_rtc.
/// Existing constructors and WallTimeClock usage remain source-compatible.
/// See roo_rtc for fresh-read status, cache control, and stricter time
/// validity.
#include "roo_rtc/ds3231.h"

namespace roo_time {
/// DS3231 wall clock using an application-initialized roo_io I2C bus.
/// Defaults to UTC and a ten-second cache interval; accepts TwoWire& on
/// Arduino. Call init() on native ESP-IDF. now() returns unset on bus/calendar
/// errors or oscillator-stop flags. set() returns false on failure and
/// invalidates cache; hardware may be partially updated. Only years 2000-2099
/// are supported.
using Ds3231Clock = roo_rtc::Ds3231Clock;
}  // namespace roo_time
