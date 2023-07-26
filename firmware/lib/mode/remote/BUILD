load("//bazel/rules:avr.bzl", "avr_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "remote",
    deps = [
        "//firmware/lib/control",
        "//firmware/lib/maze",
        "//firmware/lib/mode/explore",
    ],
)
