package(default_visibility = ["//visibility:public"])

cc_library(
    name = "utils",
    srcs = glob([
        "*.c",
        "*.cc",
        "*_impl.h",
        "*_impl.hh",
    ]),
    hdrs = glob(
        include = [
            "*.h",
            "*.hh",
        ],
        exclude = [
            "*_impl.h",
            "*_impl.hh",
        ],
    ),
    target_compatible_with = select({
        "//bazel/platforms/avr:yes": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = ["//firmware/platform"],
)
