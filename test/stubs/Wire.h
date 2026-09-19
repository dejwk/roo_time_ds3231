#pragma once

#include <stddef.h>
#include <stdint.h>

#include <vector>

/// Scripted Wire boundary for short receives and transmit-buffer exhaustion.
class TwoWire {
 public:
  /// Reports successful bus setup without hardware.
  bool begin(int = -1, int = -1, uint32_t = 0) { return true; }
  /// Starts a recorded write to the specified address.
  void beginTransmission(uint8_t address) {
    last_address = address;
    ++begins;
    sent.clear();
  }

  /// Queues one byte subject to the scripted buffer limit.
  size_t write(uint8_t value) { return write(&value, 1); }

  /// Records the bytes accepted by the scripted buffer limit.
  size_t write(const uint8_t* data, size_t size) {
    const size_t count = size < write_limit ? size : write_limit;
    sent.insert(sent.end(), data, data + count);
    return count;
  }

  /// Records STOP intent and returns the scripted transfer status.
  uint8_t endTransmission(bool stop = true) {
    last_stop = stop;
    ++ends;
    return status;
  }

  /// Starts a scripted receive and reports its configured byte count.
  size_t requestFrom(uint8_t address, uint8_t size) {
    last_address = address;
    requested = size;
    ++requests;
    index = 0;
    return reported_count;
  }

  /// Returns the next scripted byte or -1 when exhausted.
  int read() {
    ++reads;
    return index < received.size() ? received[index++] : -1;
  }

  bool last_stop = true;
  size_t write_limit = 8;
  uint8_t status = 0;
  size_t reported_count = 7;
  std::vector<int> received = {0x56, 0x34, 0x12, 3, 0x15, 0x06, 0x24};
  std::vector<uint8_t> sent;
  size_t index = 0;
  int begins = 0;
  int ends = 0;
  int requests = 0;
  int reads = 0;
  uint8_t last_address = 0;
  uint8_t requested = 0;
};

extern TwoWire Wire;
