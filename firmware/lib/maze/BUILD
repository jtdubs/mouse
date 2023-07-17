package(default_visibility = ["//visibility:public"])

cc_library(
    name = "maze",
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
    ],
)
