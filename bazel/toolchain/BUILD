load(":cc_toolchain_config.bzl", "cc_toolchain_config")

package(default_visibility = ["//visibility:public"])

filegroup(name = "empty")

toolchain(
    name = "atmega328p_toolchain",
    exec_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
    target_compatible_with = [
        "//bazel/platforms/avr:yes",
        "//bazel/platforms/cpu:atmega328p",
    ],
    toolchain = ":atmega328p_cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

cc_toolchain(
    name = "atmega328p_cc_toolchain",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    toolchain_config = ":atmega328p_cc_toolchain_config",
    toolchain_identifier = "atmega328p-cc-toolchain",
)

cc_toolchain_config(
    name = "atmega328p_cc_toolchain_config",
    freq = "16000000UL",
    mmcu = "atmega328p",
)

toolchain(
    name = "atmega4809_toolchain",
    exec_compatible_with = [
        "@platforms//cpu:x86_64",
        "@platforms//os:linux",
    ],
    target_compatible_with = [
        "//bazel/platforms/avr:yes",
        "//bazel/platforms/cpu:atmega4809",
    ],
    toolchain = ":atmega4809_cc_toolchain",
    toolchain_type = "@bazel_tools//tools/cpp:toolchain_type",
)

cc_toolchain(
    name = "atmega4809_cc_toolchain",
    all_files = ":empty",
    compiler_files = ":empty",
    dwp_files = ":empty",
    linker_files = ":empty",
    objcopy_files = ":empty",
    strip_files = ":empty",
    supports_param_files = 0,
    toolchain_config = ":atmega4809_cc_toolchain_config",
    toolchain_identifier = "atmega4809-cc-toolchain",
)

cc_toolchain_config(
    name = "atmega4809_cc_toolchain_config",
    freq = "20000000UL",
    mmcu = "atmega4809",
)
