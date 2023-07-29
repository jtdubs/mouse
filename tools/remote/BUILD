package(default_visibility = ["//visibility:public"])

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
