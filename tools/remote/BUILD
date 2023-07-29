load("@com_grail_bazel_compdb//:defs.bzl", "compilation_database")
load("@com_grail_bazel_output_base_util//:defs.bzl", "OUTPUT_BASE")

cc_library(
    name = "cli11",
    hdrs = ["@cli11//file"],
    include_prefix = "cli11",
    strip_include_prefix = "/file",
)

cc_binary(
    name = "remote",
    srcs = glob([
        "*.cc",
        "*.hh",
    ]),
    linkopts = [
        "-L/usr/local/lib",
    ],
    deps = [
        ":cli11",
        "//tools/remote/lib/remote",
        "//tools/remote/lib/ui",
        "@simavr//:lib",
        "@simavrparts//:lib",
    ],
)

compilation_database(
    name = "compdb",
    output_base = OUTPUT_BASE,
    targets = [":remote"],
)
