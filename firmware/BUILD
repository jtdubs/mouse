load("//bazel/rules:avr.bzl", "avr_binary")

platform_library = select(
    {
        "//bazel/platforms/cpu:atmega328p": ["//firmware/platform/atmega328p"],
        "//bazel/platforms/cpu:atmega4809": ["//firmware/platform/atmega4809"],
        "//conditions:default": [],
    },
)

avr_binary(
    name = "mouse",
    deps = [
        "//firmware/lib/control",
        "//firmware/lib/mode/explore",
        "//firmware/lib/mode/remote",
        "//firmware/lib/report",
        "//firmware/lib/utils",
        "//firmware/platform",
    ] + platform_library,
)
