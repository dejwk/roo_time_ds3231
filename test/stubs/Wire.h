#pragma once

#include <stddef.h>
#include <stdint.h>

#include <vector>

// Scripted Wire boundary for transport failures that the device emulator cannot
// express, such as a short receive count or transmit-buffer exhaustion.
class TwoWire {
public:
  void beginTransmission(uint8_t address) {
    last_address = address;
    ++begins;
    sent.clear();
  }

  size_t write(uint8_t value) { return write(&value, 1); }

  size_t write(const uint8_t *data, size_t size) {
    const size_t count = size < write_limit ? size : write_limit;
    sent.insert(sent.end(), data, data + count);
    return count;
  }

  uint8_t endTransmission() {
    ++ends;
    return status;
  }

  size_t requestFrom(uint8_t address, uint8_t size) {
    last_address = address;
    requested = size;
    ++requests;
    index = 0;
    return reported_count;
  }

  int read() {
    ++reads;
    return index < received.size() ? received[index++] : -1;
  }

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
