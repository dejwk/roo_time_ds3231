# roo_time_ds3231 2.0.0

- Add native ESP-IDF support through `roo_io` I2C, preserving existing Arduino constructor calls. ESP-IDF requires bus setup and `clock.init()` before use.
- Add an ESP-IDF example and tests for initialization, read/write operations, error recovery, and cache invalidation.
- Upgrade dependencies to `roo_time` 2.0.0, `roo_io` 2.3.0, and `roo_testing` 2.1.2; refresh Bazel and CI dependencies.
- Resolve build warnings and expand setup documentation.

---

# [roo_time_ds3231 1.1.9](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.9)

Published 2026-09-18.

- Improved I²C error handling: now() returns WallTime::Unset() on failed or incomplete reads. Failed readings are not cached, allowing the next call to retry.
- set() now returns a bool indicating write success. Failed writes invalidate the cached time.
- Added validation of RTC data, rejecting invalid BCD values, out-of-range fields, reserved bits, and impossible dates, including invalid leap days.
- Added correct decoding of 12-hour mode and the century bit when reading dates from 2000–2199.
- Updated to roo_time 1.5.0, replacing constructor TimeZone parameters with UtcOffset.
- Updated the example and documentation to demonstrate error handling, and added regression tests for I²C failures and date validation.
Migration: Update explicit TimeZone usage to UtcOffset. Check now().isSet() before converting to DateTime, and check the return value of set() to detect write failures. Existing calls that ignore set()’s return value still compile.

---

# [roo_time_ds3231 1.1.8](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.8)

Published 2026-08-29.

This release updates the library’s dependency baseline.

- Requires `roo_time` 1.4.7 or newer.
- Updates the Bazel test dependency to `roo_testing` 2.1.0.
- Bumps package metadata to version 1.1.8.

No driver API or behavioral changes are included in this release.

---

# [roo_time_ds3231 1.1.7](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.7)

Published 2026-08-21.

roo_time_ds3231 1.1.7

* Added a runnable host-emulated DS3231 example: bazel run //examples/simple
* Updated host-build support to roo_testing 2.0, including Arduino ESP32 profiles and AddressSanitizer configuration.
* Improved CI reliability and standardized the Bazel/Bazelisk setup.
* Updated dependencies to roo_testing 2.0.1 and roo_time 1.4.6.
* No library API changes are included in this release.


---

# [roo_time_ds3231 1.1.6](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.6)

Published 2026-02-26.

* Updated dependencies.
* Builds without warnings now.

---

# [roo_time_ds3231 1.1.5](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.5)

Published 2026-01-26.

Updated test config to account for Bazel behavior change.

**Full Changelog**: https://github.com/dejwk/roo_time_ds3231/compare/1.1.4...1.1.5

---

# [roo_time_ds3231 1.1.4](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.4)

Published 2026-01-06.

* Updated dependencies, particularly roo_time which now works with Raspberry Pi Pico.

**Full Changelog**: https://github.com/dejwk/roo_time_ds3231/compare/1.1.3...1.1.4

---

# [roo_time_ds3231 1.1.3](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.3)

Published 2025-11-12.

Updating dependencies only.

**Full Changelog**: https://github.com/dejwk/roo_time_ds3231/compare/1.1.2...1.1.3

---

# [roo_time_ds3231 1.1.2](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.2)

Published 2025-10-30.

Picking up the updated dependencies.

---

# [roo_time_ds3231 1.1.1](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.1)

Published 2025-10-30.

Better continuous integration, and a .gitignore file.

---

# [roo_time_ds3231 1.1.0](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.1.0)

Published 2025-10-05.

* Added a regression test, and integrated with roo_testing as a dependent library.

**Full Changelog**: https://github.com/dejwk/roo_time_ds3231/compare/1.0.0...1.1.0


---

# [roo_time_ds3231 1.0.0](https://github.com/dejwk/roo_time_ds3231/releases/tag/1.0.0)

Published 2024-11-04.

Initial release.

---

