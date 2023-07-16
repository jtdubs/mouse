load(":cc_toolchain_config.bzl", "cc_toolchain_config")

package(default_visibility = ["//visibility:public"])

filegroup(name = "empty")

toolchain(
    name = "avr_toolchain",
    exec_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
    target_compatible_with = [
        "//bazel/platforms:atmega328p",
    ],
    toolchain = ":avr_cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

cc_toolchain(
    name = "avr_cc_toolchain",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    toolchain_config = ":avr_cc_toolchain_config",
    toolchain_identifier = "avr-cc-toolchain",
)

cc_toolchain_config(
    name = "avr_cc_toolchain_config",
    freq = "16000000UL",
    mmcu = "atmega328p",
)
