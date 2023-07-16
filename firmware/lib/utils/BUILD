package(default_visibility = ["//visibility:public"])

cc_library(
    name = "utils",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    target_compatible_with = select({
        "//bazel/platforms:atmega328p": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = ["//firmware/platform"],
)
