package(default_visibility = ["//visibility:public"])

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
        "//tools/remote/lib/remote",
        "//tools/remote/lib/ui",
        "@CLI11",
        "@simavr//:lib",
        "@simavrparts//:lib",
    ],
)
