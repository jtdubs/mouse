# AVR Toolchain
register_toolchains(
    "//bazel/toolchain:atmega328p_toolchain",
    "//bazel/toolchain:atmega4809_toolchain",
)

# pkg-config support

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

http_archive(
    name = "bazel_pkg_config",
    strip_prefix = "bazel_pkg_config-master",
    urls = ["https://github.com/cherrry/bazel_pkg_config/archive/master.zip"],
)

load("@bazel_pkg_config//:pkg_config.bzl", "pkg_config")

pkg_config(name = "simavr")

pkg_config(name = "simavrparts")

pkg_config(name = "glfw3")

pkg_config(name = "gl")

# foreign libraries

http_archive(
    name = "rules_foreign_cc",
    sha256 = "2a4d07cd64b0719b39a7c12218a3e507672b82a97b98c6a89d38565894cf7c51",
    strip_prefix = "rules_foreign_cc-0.9.0",
    url = "https://github.com/bazelbuild/rules_foreign_cc/archive/0.9.0.tar.gz",
)

load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies()

# third_party libraries

new_local_repository(
    name = "CLI11",
    build_file = "third_party/BUILD.CLI11",
    path = "third_party/CLI11",
)

new_local_repository(
    name = "ELFIO",
    build_file = "third_party/BUILD.ELFIO",
    path = "third_party/ELFIO",
)

new_local_repository(
    name = "imgui",
    build_file = "third_party/BUILD.imgui",
    path = "third_party/imgui",
)

new_local_repository(
    name = "libspng",
    build_file = "third_party/BUILD.libspng",
    path = "third_party/libspng",
)

new_local_repository(
    name = "stb",
    build_file = "third_party/BUILD.stb",
    path = "third_party/stb",
)
