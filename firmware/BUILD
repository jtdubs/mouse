load("//bazel/rules:objcopy.bzl", "objcopy")
load("//bazel/rules:objdump.bzl", "objdump")

platform_library = select(
    {
        "//bazel/platforms/cpu:atmega328p": ["//firmware/platform/atmega328p"],
        "//bazel/platforms/cpu:atmega4809": ["//firmware/platform/atmega4809"],
        "//conditions:default": [],
    },
)

cc_binary(
    name = "mouse",
    srcs = glob(["*.c"]),
    linkstatic = True,
    target_compatible_with = select({
        "//bazel/platforms/avr:yes": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//firmware/lib/control",
        "//firmware/lib/mode/explore",
        "//firmware/lib/mode/remote",
        "//firmware/lib/report",
        "//firmware/lib/utils",
        "//firmware/platform",
    ] + platform_library,
)

objcopy(
    name = "mouse_hex",
    src = ":mouse",
)

objdump(
    name = "mouse_s",
    src = ":mouse",
)
