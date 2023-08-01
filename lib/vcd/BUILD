package(default_visibility = ["//visibility:public"])

cc_library(
    name = "vcd",
    srcs = glob(
        include = [
            "*.cc",
            "*_impl.hh",
        ],
        exclude = [
            "*_test.cc",
            "*_test.hh",
        ],
    ),
    hdrs = glob(
        include = ["*.hh"],
        exclude = ["*_impl.hh"],
    ),
)

cc_test(
    name = "vcd_test",
    size = "small",
    srcs = ["vcdwriter_test.cc"],
    deps = [
        ":vcd",
        "@com_google_googletest//:gtest_main",
    ],
)
