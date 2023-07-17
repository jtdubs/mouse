package(default_visibility = ["//visibility:public"])

cc_library(
    name = "atmega328p",
    srcs = glob([
        "*.c",
        "*_impl.h",
    ]),
    hdrs = glob(
        include = ["*.h"],
        exclude = ["*_impl.h"],
    ),
    target_compatible_with = select({
        "//bazel/platforms/cpu:atmega328p": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//firmware/lib/utils",
        "//firmware/platform",
    ],
)
