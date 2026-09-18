#include "gtest/gtest.h"
#include "roo_time_ds3231.h"

TwoWire Wire;

namespace roo_time {
namespace {
WallTime ExpectedTime() {
  return DateTime(2024, 6, 15, 12, 34, 56, 0, timezone::UTC).wallTime();
}
}  // namespace

// Verifies success decodes a complete read and then uses the cached reading.
TEST(WireErrors, SuccessfulReadAndCache) {
  TwoWire wire;
  Ds3231Clock rtc(wire, timezone::UTC, Hours(1));
  EXPECT_EQ(ExpectedTime(), rtc.now());
  EXPECT_EQ(0x68, wire.last_address);
  EXPECT_EQ(7, wire.requested);
  EXPECT_EQ((std::vector<uint8_t>{0}), wire.sent);
  wire.status = 2;
  EXPECT_TRUE(rtc.now().isSet());
  EXPECT_EQ(1, wire.begins);
  EXPECT_EQ(7, wire.reads);
}

// Verifies NACK, other errors, and timeout stop the read and allow recovery.
TEST(WireErrors, RegisterSelectionFailure) {
  for (uint8_t status : {1, 2, 3, 4, 5}) {
    TwoWire wire;
    Ds3231Clock rtc(wire);
    wire.status = status;
    EXPECT_FALSE(rtc.now().isSet());
    EXPECT_EQ(1, wire.ends);
    EXPECT_EQ(0, wire.requests);
    wire.status = 0;
    EXPECT_EQ(ExpectedTime(), rtc.now());
    EXPECT_EQ(2, wire.begins);
  }
}

// Verifies even a failed register-address enqueue finishes the transmission.
TEST(WireErrors, RegisterAddressEnqueueFailure) {
  TwoWire wire;
  Ds3231Clock rtc(wire);
  wire.write_limit = 0;
  EXPECT_FALSE(rtc.now().isSet());
  EXPECT_EQ(1, wire.ends);
  EXPECT_EQ(0, wire.requests);
  wire.write_limit = 8;
  EXPECT_EQ(ExpectedTime(), rtc.now());
}

// Verifies incomplete reads never become timestamps, including after success.
TEST(WireErrors, ShortReadAndRecovery) {
  for (size_t count = 0; count < 7; ++count) {
    TwoWire wire;
    Ds3231Clock rtc(wire, timezone::UTC, Micros(0));
    ASSERT_EQ(ExpectedTime(), rtc.now());
    wire.reported_count = count;
    EXPECT_FALSE(rtc.now().isSet());
    EXPECT_EQ(7, wire.reads);
    wire.reported_count = 7;
    EXPECT_EQ(ExpectedTime(), rtc.now());
  }
}

// Verifies read() failure is detected even if requestFrom reported seven bytes.
TEST(WireErrors, MissingReceivedByte) {
  for (size_t count = 0; count < 7; ++count) {
    TwoWire wire;
    Ds3231Clock rtc(wire);
    wire.received.resize(count);
    EXPECT_FALSE(rtc.now().isSet());
    EXPECT_EQ(static_cast<int>(count + 1), wire.reads);
  }
}

// Verifies writes are checked byte-for-byte and a success supplies the cache.
TEST(WireErrors, SuccessfulWrite) {
  TwoWire wire;
  Ds3231Clock rtc(wire);
  const WallTime time = ExpectedTime();
  ASSERT_TRUE(rtc.set(time));
  EXPECT_EQ((std::vector<uint8_t>{0, 0x56, 0x34, 0x12, 6, 0x15, 0x06, 0x24}),
            wire.sent);
  EXPECT_EQ(1, wire.ends);
  EXPECT_EQ(time, rtc.now());
  EXPECT_EQ(0, wire.requests);
}

// Verifies failed writes invalidate a fresh cache and do not cache the new
// time.
TEST(WireErrors, FailedWriteInvalidatesCache) {
  for (uint8_t status : {1, 2, 3, 4, 5}) {
    TwoWire wire;
    Ds3231Clock rtc(wire);
    ASSERT_TRUE(rtc.set(ExpectedTime()));
    wire.status = status;
    EXPECT_FALSE(rtc.set(ExpectedTime() + Hours(1)));
    EXPECT_FALSE(rtc.now().isSet());
    wire.status = 0;
    EXPECT_EQ(ExpectedTime(), rtc.now());
    EXPECT_TRUE(rtc.set(ExpectedTime() + Hours(1)));
    EXPECT_EQ(ExpectedTime() + Hours(1), rtc.now());
  }
}

// Verifies partial transmit buffers fail even when endTransmission succeeds.
TEST(WireErrors, ShortWriteInvalidatesCache) {
  for (size_t count = 0; count < 8; ++count) {
    TwoWire wire;
    Ds3231Clock rtc(wire);
    ASSERT_TRUE(rtc.set(ExpectedTime()));
    wire.write_limit = count;
    EXPECT_FALSE(rtc.set(ExpectedTime() + Hours(1)));
    EXPECT_EQ(2, wire.ends);
    wire.write_limit = 8;
    EXPECT_EQ(ExpectedTime(), rtc.now());
    EXPECT_EQ(1, wire.requests);
  }
}

// Verifies invalid BCD, ranges, reserved bits, and impossible dates are
// rejected.
TEST(DateValidation, InvalidReadingsAndRecovery) {
  struct InvalidField {
    size_t index;
    int value;
  };
  const InvalidField invalid[] = {
      {0, 0x0A}, {0, 0x60}, {0, 0x80}, {1, 0x1F}, {1, 0x60}, {1, 0x80},
      {2, 0x24}, {2, 0x1A}, {2, 0x80}, {2, 0x40}, {2, 0x53}, {2, 0x6A},
      {4, 0x00}, {4, 0x31}, {4, 0x32}, {4, 0x1A}, {4, 0x41}, {5, 0x00},
      {5, 0x13}, {5, 0x1A}, {5, 0x26}, {5, 0x46}, {6, 0x1A}, {6, 0xA0}};
  for (const InvalidField& field : invalid) {
    SCOPED_TRACE(field.index);
    SCOPED_TRACE(field.value);
    TwoWire wire;
    Ds3231Clock rtc(wire, timezone::UTC, Micros(0));
    ASSERT_EQ(ExpectedTime(), rtc.now());
    wire.received[field.index] = field.value;
    EXPECT_FALSE(rtc.now().isSet());
    wire.received = {0x56, 0x34, 0x12, 3, 0x15, 0x06, 0x24};
    EXPECT_EQ(ExpectedTime(), rtc.now());
  }
}

// Verifies Gregorian leap years, including the non-leap century 2100.
TEST(DateValidation, CalendarBoundaries) {
  struct Case {
    int year;
    int month;
    int day;
    bool valid;
  };
  const Case cases[] = {
      {2000, 2, 29, true},  {2024, 2, 29, true}, {2023, 2, 29, false},
      {2100, 2, 29, false}, {2104, 2, 29, true}, {2024, 2, 30, false},
      {2024, 4, 31, false}, {2024, 4, 30, true}, {2099, 12, 31, true},
      {2100, 1, 1, true},   {2199, 12, 31, true}};
  for (const Case& c : cases) {
    SCOPED_TRACE(c.year);
    TwoWire wire;
    wire.received[4] = (c.day / 10) * 16 + c.day % 10;
    wire.received[5] =
        ((c.month / 10) * 16 + c.month % 10) | (c.year >= 2100 ? 0x80 : 0);
    wire.received[6] = ((c.year % 100) / 10) * 16 + c.year % 10;
    Ds3231Clock rtc(wire, UtcOffset(Hours(2)));
    const WallTime result = rtc.now();
    EXPECT_EQ(c.valid, result.isSet());
    if (c.valid) {
      EXPECT_EQ(
          DateTime(c.year, c.month, c.day, 12, 34, 56, 0, UtcOffset(Hours(2)))
              .wallTime(),
          result);
    }
  }
}

// Verifies every hour in both modes, particularly 12 AM and 12 PM.
TEST(DateValidation, HourModes) {
  for (int hour = 0; hour < 24; ++hour) {
    for (bool twelve_hour : {false, true}) {
      TwoWire wire;
      const int displayed =
          twelve_hour ? (hour % 12 == 0 ? 12 : hour % 12) : hour;
      wire.received[2] = (displayed / 10) * 16 + displayed % 10;
      if (twelve_hour) wire.received[2] |= 0x40 | (hour >= 12 ? 0x20 : 0);
      Ds3231Clock rtc(wire);
      EXPECT_EQ(
          DateTime(2024, 6, 15, hour, 34, 56, 0, timezone::UTC).wallTime(),
          rtc.now());
    }
  }
}
}  // namespace roo_time
