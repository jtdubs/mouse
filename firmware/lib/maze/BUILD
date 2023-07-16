package(default_visibility = ["//visibility:public"])

cc_library(
    name = "maze",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    target_compatible_with = select({
        "//bazel/platforms/avr:yes": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//firmware/lib/control",
    ],
)
