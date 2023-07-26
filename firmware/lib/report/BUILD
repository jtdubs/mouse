load("//bazel/rules:avr.bzl", "avr_library")

package(default_visibility = ["//visibility:public"])

avr_library(
    name = "report",
    deps = [
        "//firmware/lib/control",
        "//firmware/lib/maze",
        "//firmware/lib/mode/remote",
        "//firmware/platform",
    ],
)
