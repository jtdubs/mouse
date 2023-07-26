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
        "//firmware/lib/control:control_headers",
        "//firmware/lib/utils:utils_headers",
        "//tools/sim/lib/sim",
        "@gl//:lib",
        "@glfw3//:lib",
        "@imgui",
        "@stb",
    ],
)
