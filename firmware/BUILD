load("//bazel/rules:objcopy.bzl", "objcopy")
load("//bazel/rules:objdump.bzl", "objdump")

cc_binary(
    name = "mouse",
    srcs = glob(["*.c"]),
    linkstatic = True,
    target_compatible_with = select({
        "//bazel/platforms:atmega328p": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
    deps = [
        "//firmware/lib/control",
        "//firmware/lib/mode/explore",
        "//firmware/lib/mode/remote",
        "//firmware/lib/report",
        "//firmware/lib/utils",
        "//firmware/platform",
        "//firmware/platform/nano",
    ],
)

objcopy(
    name = "mouse_hex",
    src = ":mouse",
    target_compatible_with = select({
        "//bazel/platforms:atmega328p": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)

objdump(
    name = "mouse_s",
    src = ":mouse",
    target_compatible_with = select({
        "//bazel/platforms:atmega328p": [],
        "//conditions:default": ["@platforms//:incompatible"],
    }),
)
