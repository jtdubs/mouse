load("//bazel/rules:avr.bzl", "avr_binary", "avr_library")

platform_library = select(
    {
        "//bazel/platforms/cpu:atmega328p": ["//firmware/platform/atmega328p"],
        "//bazel/platforms/cpu:atmega4809": ["//firmware/platform/atmega4809"],
        "//conditions:default": [],
    },
)

cc_library(
    name = "config",
    srcs = [],
    hdrs = ["config.hh"],
    visibility = ["//visibility:public"],
)

avr_binary(
    name = "mouse",
    srcs = ["main.cc"],
    deps = [
        "//firmware/lib/control",
        "//firmware/lib/mode/explore",
        "//firmware/lib/mode/remote",
        "//firmware/lib/report",
        "//firmware/lib/utils",
        "//firmware/platform",
    ] + platform_library,
)
