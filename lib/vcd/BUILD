package(default_visibility = ["//visibility:public"])

cc_library(
    name = "vcd",
    srcs = glob([
        "*.cc",
        "*_impl.hh",
    ]),
    hdrs = glob(
        include = ["*.hh"],
        exclude = ["*_impl.hh"],
    ),
)
