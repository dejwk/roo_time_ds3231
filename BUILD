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

# Compile the production driver against a scripted Wire boundary for faults.
cc_test(
    name = "wire_errors_test",
    size = "small",
    target_compatible_with = ["@roo_testing//roo_testing/platforms:arduino"],
    srcs = [
        "src/roo_time_ds3231.cpp",
        "src/roo_time_ds3231.h",
        "test/stubs/Wire.h",
        "@roo_io//:arduino_i2c_sources",
        "test/wire_errors_test.cpp",
    ],
    includes = ["test/stubs", "src"],
    linkstatic = True,
    deps = ["@roo_io//:i2c_headers", "@roo_time", "@googletest//:gtest_main"],
)

cc_test(
    name = "idf_clock_test",
    size = "small",
    linkstatic = True,
    srcs = ["test/idf_clock_test.cpp"],
    deps = [
        ":roo_time_ds3231",
        "@roo_io//test/i2c:idf_fake",
        "@googletest//:gtest_main",
    ],
)
