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
        "//lib/roo_time",
        "//roo_testing/frameworks/arduino-esp32-2.0.4/libraries/Wire",
    ],
)
