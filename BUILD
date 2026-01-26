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
        "@roo_testing//roo_testing/frameworks/arduino-esp32-2.0.4/libraries/Wire",
        "@roo_time",
    ],
)

cc_test(
    name = "roo_time_ds3231_test",
    size = "small",
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
)
