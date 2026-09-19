See [roo_time](http://github.com/dejwk/roo_time).

## Host emulation

Host builds support Arduino and native ESP-IDF ESP32 profiles. With Bazelisk 1.21
or newer, a plain command defaults to that profile and prints a notice:

    bazel test ...
    bazel test ... --config=asan
    bazel test ... --config=roo_testing_arduino_esp32

The bundled sketch is a first-class runnable target (stop it with Ctrl-C):

    bazel run //examples/simple

The files under .roo_testing/bazelrc/esp32 are vendored from roo_testing;
follow their canonical-source headers when refreshing them.

## I2C errors

On Arduino, call `Wire.begin(...)` before using the clock; existing default and
`TwoWire&` constructor calls continue to work. On ESP-IDF, initialize the bus and
call `clock.init()` before reading or setting the clock (see below). `now()` returns
`WallTime::Unset()` when a required I2C transaction fails or returns incomplete
data. Check `isSet()` before converting to `DateTime`. Successful readings may
be extrapolated for `max_uptime_trusted`; cached reads do not check bus health.
Failed reads are not cached, so the next call retries.

`set(time)` returns `true` only when all bytes are transferred successfully. On failure it returns `false` and invalidates the
cached reading. A failed write may partially update the RTC; callers should retry
the complete write when appropriate. Existing calls that ignore the return value
still compile, but should check it to detect errors.

RTC reads also reject invalid BCD digits, out-of-range time fields, reserved bits
in date/time fields, and impossible Gregorian dates (including invalid leap days).
Both 12-hour and 24-hour modes are decoded. The century bit selects 2000-2099
(clear) or 2100-2199 (set). The weekday register is ignored; the date determines
the weekday. Invalid readings return unset and are not cached.
This checks the returned calendar data, not the oscillator-stop flag or clock
synchronization status.

## Native ESP-IDF

The driver uses `roo_io/i2c/i2c.h`; no Arduino headers or Wire library are needed
in a native IDF build. Bus setup belongs to the application:

```cpp
roo_io::I2cMasterBusHandle bus;
if (!bus.init(18, 19, 100000)) return;
roo_time::Ds3231Clock clock(bus);
if (!clock.init()) return;
auto time = clock.now();
if (!time.isSet()) return;
```

Keep the clock alive for as long as it is needed. Bus copies and devices retain
an owned bus until the last reference is destroyed. To reuse an existing IDF bus,
construct `roo_io::I2cMasterBusHandle(native_handle)` and keep the external bus
alive until the clock is destroyed. `clock.init()` is idempotent and can be
retried after a setup failure. Transfers before successful initialization fail.

The native adapter uses IDF's modern `driver/i2c_master.h` API (including
`i2c_master_get_bus_handle`), not the legacy I2C driver. Include IDF's
`esp_driver_i2c` component in the firmware build, and compile the native
`roo_io` I2C implementation alongside `roo_time` and this library. Do not define
`ARDUINO` in that build.

Host checks:

```sh
bazel test --config=roo_testing_idf_esp32 //:roo_time_ds3231_test //:idf_clock_test
bazel run --config=roo_testing_idf_esp32 //examples/espidf/simple:simple
```

Stop the host example with Ctrl-C after its status message.

The integration test and example use roo_testing's pin-routed DS3231 emulator
through the modern IDF I2C master API. Separate scripted-boundary tests check
initialization failures and cache invalidation. These checks do not establish
electrical behavior on hardware. Until the dependencies are released, pass local
`--override_module` flags for `roo_io`, `roo_time`, and `roo_testing` when running
from this repository; dependency-module overrides are not inherited by Bazel.
