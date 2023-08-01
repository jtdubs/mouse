load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive")

# AVR Toolchain
register_toolchains(
    "//bazel/toolchain:atmega328p_toolchain",
    "//bazel/toolchain:atmega4809_toolchain",
)

# google test framework

http_archive(
    name = "com_google_googletest",
    strip_prefix = "googletest-1.13.0",
    urls = ["https://github.com/google/googletest/archive/refs/tags/v1.13.0.tar.gz"],
)

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

new_local_repository(
    name = "simavr",
    build_file = "third_party/BUILD.simavr",
    path = "third_party/simavr",
)

new_local_repository(
    name = "glfw",
    build_file = "third_party/BUILD.glfw",
    path = "third_party/glfw",
)
