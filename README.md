See [roo_time](http://github.com/dejwk/roo_time).

## Host emulation

Host builds use the roo_testing 2.0 Arduino ESP32 profile. With Bazelisk 1.21
or newer, a plain command defaults to that profile and prints a notice:

    bazel test ...
    bazel test ... --config=asan
    bazel test ... --config=roo_testing_arduino_esp32

The bundled sketch is a first-class runnable target (stop it with Ctrl-C):

    bazel run //examples/simple

The files under .roo_testing/bazelrc/esp32 are vendored from roo_testing;
follow their canonical-source headers when refreshing them.

## I2C errors

Call `Wire.begin(...)` before using the clock. `now()` returns
`WallTime::Unset()` when a required Wire transaction fails or returns incomplete
data. Check `isSet()` before converting to `DateTime`. Successful readings may
be extrapolated for `max_uptime_trusted`; cached reads do not check bus health.
Failed reads are not cached, so the next call retries.

`set(time)` returns `true` only when all bytes are queued and Wire reports a
successful transmission. On failure it returns `false` and invalidates the
cached reading. A failed write may partially update the RTC; callers should retry
the complete write when appropriate. Existing calls that ignore the return value
still compile, but should check it to detect errors.
