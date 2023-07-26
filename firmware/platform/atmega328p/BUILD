load("//bazel/rules:avr.bzl", "avr_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "atmega328p",
    platform = "//bazel/platforms/cpu:atmega328p",
    deps = [
        "//firmware/lib/utils",
        "//firmware/platform",
    ],
)
