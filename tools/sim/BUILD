package(default_visibility = ["//visibility:public"])

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
    linkstatic = True,
    deps = [
        "//tools/sim/lib/sim",
        "//tools/sim/lib/ui",
        "@CLI11",
        "@simavr",
    ],
)
