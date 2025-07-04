
#include "roo_time_ds3231.h"

namespace roo_time {

namespace {

static const uint8_t kDs3231Addr = 0x68;

enum Registers {
  kRegTime = 0x00,
  kRegControl = 0x0E,
  kRegStatus = 0x0F,
  kRegTemperature = 0x11
};

uint8_t Dec2bcd(uint8_t v) { return ((v / 10) * 16) + (v % 10); }
uint8_t Bcd2dec(uint8_t v) { return ((v / 16) * 10) + (v % 16); }

}  // namespace

Ds3231Clock::Ds3231Clock(TimeZone tz, Interval max_uptime_trusted)
    : Ds3231Clock(Wire, tz, max_uptime_trusted) {}

Ds3231Clock::Ds3231Clock(TwoWire& wire, TimeZone tz,
                         Interval max_uptime_trusted)
    : wire_(wire),
      tz_(tz),
      max_uptime_trusted_(max_uptime_trusted),
      last_reading_time_(Uptime::Now() - Hours(1)) {}

void Ds3231Clock::begin() {}

WallTime Ds3231Clock::now() const {
  Uptime now = Uptime::Now();
  Interval delta = now - last_reading_time_;
  if (delta < max_uptime_trusted_) {
    // Use delta for approximation, but round to seconds, since DS3231 only
    // has second accuracy.
    return last_reading_ + roo_time::Seconds(delta.inSeconds());
  }

  wire_.beginTransmission(kDs3231Addr);
  wire_.write(kRegTime);
  wire_.endTransmission();
  wire_.requestFrom((uint8_t)kDs3231Addr, (uint8_t)7);

  uint8_t second = Bcd2dec(wire_.read());
  uint8_t minute = Bcd2dec(wire_.read());
  uint8_t hour = Bcd2dec(wire_.read());
  uint8_t ignored_dayofweek = Bcd2dec(wire_.read());
  uint8_t day = Bcd2dec(wire_.read());
  uint8_t month = Bcd2dec(wire_.read());
  uint16_t year = Bcd2dec(wire_.read()) + 2000;

  last_reading_ =
      DateTime(year, month, day, hour, minute, second, 0, tz_).wallTime();
  last_reading_time_ = now;
  return last_reading_;
}

// Sets the clock to the specified wall time. The time will be stored in the
// clock's timezone (specified during construction).
void Ds3231Clock::set(WallTime time) {
  DateTime dt(time, tz_);

  wire_.beginTransmission(kDs3231Addr);

  wire_.write(kRegTime);

  wire_.write(Dec2bcd(dt.second()));
  wire_.write(Dec2bcd(dt.minute()));
  wire_.write(Dec2bcd(dt.hour()));
  wire_.write(Dec2bcd(dt.dayOfWeek()));
  wire_.write(Dec2bcd(dt.day()));
  wire_.write(Dec2bcd(dt.month()));
  wire_.write(Dec2bcd(dt.year() - 2000));

  wire_.endTransmission();

  last_reading_ = time;
  last_reading_time_ = Uptime::Now();
}

}  // namespace roo_time