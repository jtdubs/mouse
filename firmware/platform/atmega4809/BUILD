load("//bazel/rules:avr.bzl", "avr_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "atmega4809",
    platform = "//bazel/platforms/cpu:atmega4809",
    deps = [
        "//firmware/lib/utils",
        "//firmware/platform",
    ],
)
