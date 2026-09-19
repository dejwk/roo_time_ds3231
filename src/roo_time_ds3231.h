#pragma once

/// Umbrella header for the roo_time_ds3231 module.
///
/// Provides DS3231-backed wall-time clock implementation.

#include "roo_io/i2c/i2c.h"
#include "roo_time.h"
namespace roo_time {

/// Wall-time clock implementation backed by a DS3231 RTC.
class Ds3231Clock : public roo_time::WallTimeClock {
 public:
  /// Creates a clock using default `Wire` (Arduino) or I2C port 0 (ESP-IDF).
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
  /// @param bus I2C bus connected to the DS3231; accepts TwoWire& on Arduino.
  /// The application initializes the bus before use.
  /// @param offset UTC offset used for values returned by `now()` and expected
  /// by
  ///     `set()`.
  /// @param max_uptime_trusted Maximum uptime interval for which extrapolated
  ///     time is trusted between RTC reads.
  Ds3231Clock(roo_io::I2cMasterBusHandle bus, UtcOffset offset = timezone::UTC,
              Duration max_uptime_trusted = Seconds(10));

  /// Registers the device on an already initialized bus. Required on ESP-IDF;
  /// optional on Arduino. Idempotent; false permits retry after a setup
  /// failure.
  bool init();

  /// Returns current wall time.
  ///
  /// Reads from hardware periodically and uses uptime-based interpolation
  /// between reads to keep repeated calls inexpensive. Returns
  /// WallTime::Unset() on a failed or incomplete I2C transaction. Failed reads
  /// are not cached; the next call retries. Cached reads do not probe bus
  /// health. Invalid BCD or calendar fields also return unset. Both hour modes
  /// are decoded; the century bit selects 2000-2099 or 2100-2199. The weekday
  /// register is ignored, and the oscillator-stop flag is not checked.
  WallTime now() const override;

  /// Sets RTC wall time in the clock's configured time zone.
  ///
  /// @param time New, set wall time value to store in the RTC.
  /// Returns true only if all bytes were queued and the transaction succeeded.
  /// Failure invalidates the cache; the RTC may have been partially updated.
  bool set(WallTime time);

 private:
  roo_io::I2cSlaveDevice device_;
  UtcOffset offset_;
  Duration max_uptime_trusted_;
  mutable WallTime last_reading_ = WallTime::Unset();
  mutable Uptime last_reading_time_;
};

}  // namespace roo_time