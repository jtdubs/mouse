package(default_visibility = ["//visibility:public"])

cc_library(
    name = "remote",
    srcs = glob([
        "*.cc",
        "*_impl.h",
    ]),
    hdrs = glob(
        include = ["*.h"],
        exclude = ["*_impl.h"],
    ),
    target_compatible_with = select({
        "//bazel/platforms/avr:yes": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//firmware/lib/control",
        "//firmware/lib/maze",
        "//firmware/lib/mode/explore",
    ],
)
