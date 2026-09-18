
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

Ds3231Clock::Ds3231Clock(UtcOffset offset, Duration max_uptime_trusted)
    : Ds3231Clock(Wire, offset, max_uptime_trusted) {}

Ds3231Clock::Ds3231Clock(TwoWire& wire, UtcOffset offset,
                         Duration max_uptime_trusted)
    : wire_(wire),
      offset_(offset),
      max_uptime_trusted_(max_uptime_trusted),
      last_reading_time_(Uptime::Now() - Hours(1)) {}

WallTime Ds3231Clock::now() const {
  Uptime now = Uptime::Now();
  Duration delta = now - last_reading_time_;
  if (last_reading_.isSet() && delta < max_uptime_trusted_) {
    // Use delta for approximation, but round to seconds, since DS3231 only
    // has second accuracy.
    return last_reading_ + roo_time::Seconds(delta.inSeconds());
  }

  last_reading_ = WallTime::Unset();
  wire_.beginTransmission(kDs3231Addr);
  const size_t written = wire_.write(kRegTime);
  // Always finish the transmission, including when enqueueing failed.
  const uint8_t status = wire_.endTransmission();
  if (written != 1 || status != 0) return WallTime::Unset();
  if (wire_.requestFrom(kDs3231Addr, static_cast<uint8_t>(7)) != 7) {
    return WallTime::Unset();
  }

  uint8_t registers[7];
  for (uint8_t& value : registers) {
    const int received = wire_.read();
    if (received < 0) return WallTime::Unset();
    value = static_cast<uint8_t>(received);
  }
  const uint8_t second = Bcd2dec(registers[0]);
  const uint8_t minute = Bcd2dec(registers[1]);
  const uint8_t hour = Bcd2dec(registers[2]);
  const uint8_t day = Bcd2dec(registers[4]);
  const uint8_t month = Bcd2dec(registers[5]);
  const uint16_t year = Bcd2dec(registers[6]) + 2000;

  last_reading_ =
      DateTime(year, month, day, hour, minute, second, 0, offset_).wallTime();
  last_reading_time_ = now;
  return last_reading_;
}

// Sets the clock to the specified wall time. The time will be stored in the
// clock's timezone (specified during construction). The time must not be unset.
bool Ds3231Clock::set(WallTime time) {
  assert(time.isSet());
  DateTime dt(time, offset_);

  // A failed transaction may still have changed some RTC registers.
  last_reading_ = WallTime::Unset();
  const uint8_t registers[] = {kRegTime,
                               Dec2bcd(dt.second()),
                               Dec2bcd(dt.minute()),
                               Dec2bcd(dt.hour()),
                               Dec2bcd(dt.dayOfWeek()),
                               Dec2bcd(dt.day()),
                               Dec2bcd(dt.month()),
                               Dec2bcd(dt.year() - 2000)};
  wire_.beginTransmission(kDs3231Addr);
  const size_t written = wire_.write(registers, sizeof(registers));
  const uint8_t status = wire_.endTransmission();
  if (written != sizeof(registers) || status != 0) return false;

  last_reading_ = time;
  last_reading_time_ = Uptime::Now();
  return true;
}

}  // namespace roo_time
