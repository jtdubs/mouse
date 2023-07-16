package(default_visibility = ["//visibility:public"])

cc_library(
    name = "report",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    target_compatible_with = select({
        "//bazel/platforms:atmega328p": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//firmware/lib/control",
        "//firmware/lib/maze",
        "//firmware/lib/mode/remote",
        "//firmware/platform",
    ],
)
