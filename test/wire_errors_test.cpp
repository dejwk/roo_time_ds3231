#include "roo_time_ds3231.h"

#include "gtest/gtest.h"

TwoWire Wire;

namespace roo_time {
namespace {
WallTime ExpectedTime() {
  return DateTime(2024, 6, 15, 12, 34, 56, 0, timezone::UTC).wallTime();
}
} // namespace

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
} // namespace roo_time
