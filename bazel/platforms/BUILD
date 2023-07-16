package(default_visibility = ["//visibility:public"])

platform(
    name = "arduino_nano",
    constraint_values = [
        "//bazel/platforms/cpu:atmega328p",
        "//bazel/platforms/freq:16MHz",
        "//bazel/platforms/avr:yes",
        "@platforms//os:none",
    ],
)

platform(
    name = "arduino_nano_every",
    constraint_values = [
        "//bazel/platforms/cpu:atmega4809",
        "//bazel/platforms/freq:20MHz",
        "//bazel/platforms/avr:yes",
        "@platforms//os:none",
    ],
)
