package(default_visibility = ["//visibility:public"])

cc_library(
    name = "platform",
    hdrs = glob(["*.h"]),
    target_compatible_with = select({
        "//bazel/platforms:atmega328p": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)
