load("//bazel/rules:objcopy.bzl", "objcopy")
load("//bazel/rules:objdump.bzl", "objdump")

def avr_binary(name, srcs = None, platform = None, **kwargs):
    """Create an AVR-compatible binary.

    Args:
      name: The name of the binary.
      srcs: The source files to compile. If not specified, all .cc files in the
          current directory will be used.
      platform: The platform to use for the library. If not specified, the
          platform will be set to //bazel/platforms/avr:yes.
      **kwargs: Additional arguments to pass to cc_binary.
    """
    if not srcs:
        srcs = native.glob(["*.cc"])
    if not platform:
        platform = "//bazel/platforms/avr:yes"
    if not "target_compatible_with" in kwargs:
        kwargs["target_compatible_with"] = select({
            platform: [],
            "//conditions:default": ["@platforms//:incompatible"],
        })
    native.cc_binary(
        name = name,
        srcs = srcs,
        linkstatic = True,
        **kwargs
    )
    objcopy(
        name = name + "_hex",
        src = ":" + name,
    )
    objdump(
        name = name + "_s",
        src = ":" + name,
    )

def avr_library(srcs = None, hdrs = None, platform = None, **kwargs):
    """Create an AVR-compatible library.

    Args:
      srcs: The source files to compile. If not specified, all .cc and _impl.hh
          files in the current directory will be used.
      hdrs: The header files to include. If not specified, all .hh files in the
          current directory will be used, except for _impl.hh files.
      platform: The platform to use for the library. If not specified, the
          platform will be set to //bazel/platforms/avr:yes.
      **kwargs: Additional arguments to pass to cc_library.
    """
    if not srcs:
        srcs = native.glob(["*.c", "*.cc", "*_impl.h", "*_impl.hh"])
    if not hdrs:
        hdrs = native.glob(
            include = ["*.h", "*.hh"],
            exclude = ["*_impl.h", "*_impl.hh"],
        )
    if not platform:
        platform = "//bazel/platforms/avr:yes"
    if not "target_compatible_with" in kwargs:
        kwargs["target_compatible_with"] = select({
            platform: [],
            "//conditions:default": ["@platforms//:incompatible"],
        })
    native.cc_library(
        srcs = srcs,
        hdrs = hdrs,
        **kwargs
    )
