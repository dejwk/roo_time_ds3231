#pragma once

#include "roo_time.h"

#include <Wire.h>
namespace roo_time {

// Clock implementation that uses a DS3231 device as a time source.
class Ds3231Clock : public roo_time::WallTimeClock {
 public:
  Ds3231Clock(TimeZone tz = timezone::UTC,
              Interval max_uptime_trusted = Seconds(10));

  Ds3231Clock(TwoWire& wire,
              TimeZone tz = timezone::UTC,
              Interval max_uptime_trusted = Seconds(10));

  void begin();

  // Returns the current time. Reads from the underlying hardware, and caches
  // the result for max_uptime_trusted_, using uptime reading to interpolate.
  // This way, the method can be called very frequently, and the overhead is low
  // - it communicates over I2C only sporadically, to re-sync the clock.
  WallTime now() const override;

  // Sets the clock to the specified wall time. The time will be stored in the
  // clock's timezone (specified during construction).
  void set(WallTime time);

 private:
  TwoWire& wire_;
  TimeZone tz_;
  Interval max_uptime_trusted_;
  mutable WallTime last_reading_;
  mutable Uptime last_reading_time_;
};

}  // namespace roo_time