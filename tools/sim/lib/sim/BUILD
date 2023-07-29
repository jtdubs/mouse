package(default_visibility = ["//visibility:public"])

cc_library(
    name = "sim",
    srcs = glob([
        "*.cc",
        "*_impl.hh",
    ]),
    hdrs = glob(
        include = ["*.hh"],
        exclude = ["*_impl.hh"],
    ),
    copts = [
        "-DIMGUI_DEFINE_MATH_OPERATORS",
    ],
    deps = [
        "@ELFIO",
        "@imgui",
        "@simavr//:lib",
        "@simavrparts//:lib",
    ],
)
