# BUILD file for use with https://github.com/dejwk/roo_testing.

cc_library(
    name = "roo_time_ds3231",
    srcs = [
        "src/roo_time_ds3231.h",
        "src/roo_time_ds3231.cpp",
    ],
    includes = [
        "src",
    ],
    visibility = ["//visibility:public"],
    deps = [
        "@roo_time",
        "@roo_testing//roo_testing/frameworks/arduino-esp32-2.0.4/libraries/Wire",
    ],
)

cc_test(
    name = "roo_time_ds3231_test",
    srcs = [
        "test/roo_time_ds3231_test.cpp",
    ],
    copts = ["-Iexternal/gtest/include"],
    includes = ["src"],
    linkstatic = 1,
    deps = [
        ":roo_time_ds3231",
        "@roo_testing//:arduino_gtest_main",
        "@roo_testing//roo_testing/devices/clock/ds3231",
    ],
    size = "small",
)
