#pragma once

/// Umbrella header for the roo_time_ds3231 module.
///
/// Provides DS3231-backed wall-time clock implementation.

#include <Wire.h>

#include "roo_time.h"
namespace roo_time {

/// Wall-time clock implementation backed by a DS3231 RTC.
class Ds3231Clock : public roo_time::WallTimeClock {
public:
  /// Creates a clock using the default `Wire` bus.
  ///
  /// @param offset UTC offset used for values returned by `now()` and expected
  /// by
  ///     `set()`.
  /// @param max_uptime_trusted Maximum uptime interval for which extrapolated
  ///     time is trusted between RTC reads.
  Ds3231Clock(UtcOffset offset = timezone::UTC,
              Duration max_uptime_trusted = Seconds(10));

  /// Creates a clock using the specified I2C bus.
  ///
  /// @param wire I2C bus connected to the DS3231.
  /// @param offset UTC offset used for values returned by `now()` and expected
  /// by
  ///     `set()`.
  /// @param max_uptime_trusted Maximum uptime interval for which extrapolated
  ///     time is trusted between RTC reads.
  Ds3231Clock(TwoWire &wire, UtcOffset offset = timezone::UTC,
              Duration max_uptime_trusted = Seconds(10));

  /// Returns current wall time.
  ///
  /// Reads from hardware periodically and uses uptime-based interpolation
  /// between reads to keep repeated calls inexpensive. Returns
  /// WallTime::Unset() on a failed or incomplete Wire transaction. Failed reads
  /// are not cached; the next call retries. Cached reads do not probe bus
  /// health.
  WallTime now() const override;

  /// Sets RTC wall time in the clock's configured time zone.
  ///
  /// @param time New, set wall time value to store in the RTC.
  /// Returns true only if all bytes were queued and the transaction succeeded.
  /// Failure invalidates the cache; the RTC may have been partially updated.
  bool set(WallTime time);

private:
  TwoWire &wire_;
  UtcOffset offset_;
  Duration max_uptime_trusted_;
  mutable WallTime last_reading_ = WallTime::Unset();
  mutable Uptime last_reading_time_;
};

} // namespace roo_time