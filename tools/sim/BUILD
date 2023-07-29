package(default_visibility = ["//visibility:public"])

cc_library(
    name = "cli11",
    hdrs = ["@cli11//file"],
    include_prefix = "cli11",
    strip_include_prefix = "/file",
)

cc_binary(
    name = "sim",
    srcs = glob([
        "*.cc",
        "*.hh",
    ]),
    copts = [
        "-DIMGUI_DEFINE_MATH_OPERATORS",
    ],
    linkopts = [
        "-L/usr/local/lib",
    ],
    deps = [
        ":cli11",
        "//tools/sim/lib/sim",
        "//tools/sim/lib/ui",
        "@simavr//:lib",
        "@simavrparts//:lib",
    ],
)
