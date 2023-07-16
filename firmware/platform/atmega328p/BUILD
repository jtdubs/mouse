package(default_visibility = ["//visibility:public"])

cc_library(
    name = "atmega328p",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    target_compatible_with = select({
        "//bazel/platforms/cpu:atmega328p": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//firmware/lib/utils",
        "//firmware/platform",
    ],
)
