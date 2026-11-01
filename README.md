# roo_time_ds3231

Compatibility wrapper for the DS3231 driver maintained in
[roo_rtc](https://github.com/dejwk/roo_rtc). Existing code can keep including
`roo_time_ds3231.h` and using `roo_time::Ds3231Clock`; that name now aliases
`roo_rtc::Ds3231Clock`. New applications can use `roo_rtc/ds3231.h` directly.

The existing default, fixed-offset, and `roo_io::I2cMasterBusHandle` constructors
remain available, including implicit `TwoWire&` on Arduino. `init()`, `now()`,
and `bool set(WallTime)` remain source-compatible. This is not binary compatibility.

## Behavior changes

- Fresh reads check oscillator-stop flags and return `WallTime::Unset()` when
  hardware indicates time loss. Set time from a trusted reference to recover.
- Reads and writes support years 2000–2099. The prior reader's century-bit
  interpretation is intentionally restricted to the hardware's documented
  leap-year compensation range; unsupported writes fail before hardware access.
- `set()` invalidates the cache on success and failure. The next `now()` reads
  hardware rather than returning a directly cached, potentially fractional value.
- DS weekday writes use the conventional Sunday=1 through Saturday=7 encoding.
- Fresh status is available through `read()`; `invalidateCache()` forces the next
  `now()` to access hardware.

Transactions are checked for full completion. Invalid BCD, field bounds, and
impossible Gregorian dates return unset; 12/24-hour reads are supported and the
weekday register is ignored on reads. Failed writes can partially update hardware.
Failed reads are not cached; the next call retries. Successful readings may be
extrapolated for `max_uptime_trusted`; cached calls do not check bus health or
oscillator-stop flags. Clock instances require external serialization.

Store UTC by default; fixed offsets remain available for compatibility. Apply
local timezone and DST conversion outside the RTC driver.

## Bus setup

On Arduino, initialize Wire before use; the driver accepts the default Wire or
an explicitly supplied instance. `init()` is optional for Arduino I²C, but call
it for consistent setup/error handling. On native ESP-IDF, initialize the bus and
call the clock's `init()` before reading or setting time:

```cpp
roo_io::I2cMasterBusHandle bus;
if (!bus.init(18, 19, 100000)) return;
roo_time::Ds3231Clock clock(bus);
if (!clock.init()) return;
const roo_rtc::ReadResult result = clock.read();
if (result.status != roo_rtc::ReadStatus::kOk) return;
```

Copies of an owned native bus retain it until the last reference is destroyed.
Externally supplied native buses and Arduino Wire instances must outlive the
clock. `init()` is idempotent and retryable. Native IDF uses modern
`driver/i2c_master.h`; include `esp_driver_i2c` in firmware component dependencies
and compile the roo_rtc, roo_io I²C, and roo_time sources without `ARDUINO`.

## Host checks

```sh
bazel test //... \
  --override_module=roo_rtc=../roo_rtc \
  --override_module=roo_io=../roo_io \
  --override_module=roo_time=../roo_time \
  --override_module=roo_testing=../roo_testing
bazel test --config=roo_testing_idf_esp32 //... \
  --override_module=roo_rtc=../roo_rtc \
  --override_module=roo_io=../roo_io \
  --override_module=roo_time=../roo_time \
  --override_module=roo_testing=../roo_testing
```

Transport and calendar fault tests now live in roo_rtc. This wrapper retains
source compatibility tests, native API-boundary error tests, and an emulator
integration test. Host checks establish software behavior, not electrical behavior.
Runnable examples remain at `//examples/simple` and
`//examples/espidf/simple:simple`; stop a looping host example with Ctrl-C.
