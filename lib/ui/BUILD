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
    deps = [
        "@glfw",
        "@imgui",
        "@stb",
    ],
)
