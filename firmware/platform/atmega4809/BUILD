package(default_visibility = ["//visibility:public"])

cc_library(
    name = "atmega4809",
    srcs = glob([
        "*.c",
        "*.h",
    ]),
    target_compatible_with = select({
        "//bazel/platforms/cpu:atmega4809": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//firmware/lib/utils",
        "//firmware/platform",
    ],
)
