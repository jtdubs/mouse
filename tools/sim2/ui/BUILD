package(default_visibility = ["//visibility:public"])

cc_library(
    name = "ui",
    srcs = glob([
        "*.cc",
        "*_impl.hh",
    ]),
    hdrs = glob(
        include = ["*.hh"],
        exclude = ["*_impl.hh"],
    ),
    linkopts = [
        "-lz",
    ],
    deps = [
        "//tools/sim2/sim",
        "@gl//:lib",
        "@glfw3//:lib",
        "@imgui",
        "@stb",
    ],
)
