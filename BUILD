# BUILD file for use with https://github.com/dejwk/roo_testing.

load("@rules_cc//cc:cc_library.bzl", "cc_library")
load("@rules_cc//cc:cc_test.bzl", "cc_test")

cc_library(
    name = "roo_time_ds3231",
    srcs = [
        "src/roo_time_ds3231.cpp",
        "src/roo_time_ds3231.h",
    ],
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@roo_io//:i2c",
        "@roo_rtc//:ds3231",
        "@roo_time",
    ],
)

cc_test(
    name = "roo_time_ds3231_test",
    size = "small",
    srcs = ["test/roo_time_ds3231_test.cpp"],
    linkstatic = True,
    deps = [
        ":roo_time_ds3231",
        "@roo_testing//roo_testing/devices/clock/ds3231",
        "@roo_testing//roo_testing/microcontrollers/esp32:core",
    ] + select({
        "@roo_testing//roo_testing/platforms:is_idf": ["@roo_testing//:esp_idf_gtest_main"],
        "//conditions:default": ["@roo_testing//:arduino_gtest_main"],
    }),
)

# Transport/calendar fault coverage now lives in roo_rtc's rtc_test.
cc_test(
    name = "wire_errors_test",
    size = "small",
    srcs = ["test/wire_errors_test.cpp"],
    linkstatic = True,
    target_compatible_with = ["@roo_testing//roo_testing/platforms:arduino"],
    deps = [
        ":roo_time_ds3231",
        "@googletest//:gtest_main",
    ],
)

cc_test(
    name = "idf_clock_test",
    size = "small",
    srcs = ["test/idf_clock_test.cpp"],
    linkstatic = True,
    deps = [
        ":roo_time_ds3231",
        "@googletest//:gtest_main",
        "@roo_io//test/i2c:idf_fake",
    ],
)
