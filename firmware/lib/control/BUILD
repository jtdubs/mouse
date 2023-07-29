load("//bazel/rules:avr.bzl", "avr_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "control",
    deps = [
        "//firmware:config",
        "//firmware/lib/utils",
        "//firmware/platform",
    ],
)
