load("@com_grail_bazel_compdb//:defs.bzl", "compilation_database")
load("@com_grail_bazel_output_base_util//:defs.bzl", "OUTPUT_BASE")
load("//bazel/rules:avr.bzl", "avr_binary")

package(default_visibility = ["//visibility:public"])

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

compilation_database(
    name = "compdb",
    output_base = OUTPUT_BASE,
    targets = [
        "//firmware:mouse",
        "//firmware/lib/control",
        "//firmware/lib/maze",
        "//firmware/lib/mode/explore",
        "//firmware/lib/mode/remote",
        "//firmware/lib/report",
        "//firmware/lib/utils",
        "//firmware/platform",
        "//firmware/platform/atmega328p",
    ],
)
