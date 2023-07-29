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
    copts = [
        "-DIMGUI_DEFINE_MATH_OPERATORS",
    ],
    linkopts = [
        "-lz",
    ],
    deps = [
        "//firmware/lib/control:control_headers",
        "//firmware/lib/maze:maze_headers",
        "//firmware/lib/mode/explore:explore_headers",
        "//firmware/lib/mode/remote:remote_headers",
        "//firmware/lib/utils:utils_headers",
        "//firmware/platform:platform_headers",
        "//tools/remote/lib/remote",
        "@gl//:lib",
        "@glfw3//:lib",
        "@imgui",
        "@stb",
    ],
)
