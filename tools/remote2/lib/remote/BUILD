package(default_visibility = ["//visibility:public"])

cc_library(
    name = "remote",
    srcs = glob([
        "*.cc",
        "*_impl.hh",
    ]),
    hdrs = glob(
        include = ["*.hh"],
        exclude = ["*_impl.hh"],
    ),
    deps = [
        "@elfio",
        "@imgui",
        "@simavr//:lib",
        "@simavrparts//:lib",
    ],
)
