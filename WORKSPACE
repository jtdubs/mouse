# AVR Toolchain
register_toolchains(
    "//bazel/toolchain:atmega328p_toolchain",
    "//bazel/toolchain:atmega4809_toolchain",
)

# pkg-config support

load("@bazel_tools//tools/build_defs/repo:http.bzl", "http_archive", "http_file")
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

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

# c++ libraries

http_archive(
    name = "elfio",
    build_file_content = """
cc_library(
  name = "elfio",
  visibility = ['//visibility:public'],
  hdrs = glob(['elfio/*.hpp']),
  includes = ["."],
)
""",
    sha256 = "3307b104c205399786edbba203906de9517e36297709fe747faf9478d55fbb91",
    strip_prefix = "elfio-3.11",
    urls = [
        "https://github.com/serge1/ELFIO/releases/download/Release_3.11/elfio-3.11.tar.gz",
    ],
)

http_file(
    name = "cli11",
    downloaded_file_path = "CLI11.hh",
    sha256 = "ba83806399a66634ca8f8d292df031e5ed651315ceb9a6a09ba56f88d75f1797",
    urls = [
        "https://github.com/CLIUtils/CLI11/releases/download/v2.3.2/CLI11.hpp",
    ],
)

http_archive(
    name = "libspng",
    build_file_content = """
cc_library(
  name = "libspng",
  visibility = ['//visibility:public'],
  srcs = glob(['spng/spng.c']),
  hdrs = glob(['spng/spng.h']),
  includes = ["."],
)
""",
    sha256 = "47ec02be6c0a6323044600a9221b049f63e1953faf816903e7383d4dc4234487",
    strip_prefix = "libspng-0.7.4",
    urls = [
        "https://github.com/randy408/libspng/archive/refs/tags/v0.7.4.tar.gz",
    ],
)

git_repository(
    name = "imgui",
    branch = "docking",
    build_file_content = """
cc_library(
      name = "imgui",
      srcs = glob([
        "*.cpp",
        "backends/imgui_impl_opengl3*",
        "backends/imgui_impl_glfw*",
      ]),
      hdrs = glob([
        "*.h",
        "backends/imgui_impl_opengl3*.h",
        "backends/imgui_impl_glfw*.h",
      ]),
      includes = ["."],
      linkopts = [
        "-lglfw",
        "-lGL",
      ],
      visibility = ["//visibility:public"],
    )
""",
    remote = "git@github.com:ocornut/imgui.git",
)

git_repository(
    name = "stb",
    branch = "master",
    build_file_content = """
cc_library(
      name = "stb",
      srcs = [],
      hdrs = glob(["*.h"]),
      includes = ["."],
      visibility = ["//visibility:public"],
    )
""",
    remote = "git@github.com:nothings/stb.git",
)
# git_repository(
#     name = "simavr",
#     branch = "master",
#     build_file_content = """
# make(
#   name = "simavr",
#   args = ["-j", "`nproc`"],
#   "
# )
# """,
#     commit = "7003af00df0f89a38898896fe536c5f15ae4ef1a",
#     remote = "git@github.com:buserror/simavr.git",
# )
#

# compilation database

http_archive(
    name = "com_grail_bazel_compdb",
    strip_prefix = "bazel-compilation-database-0.5.2",
    urls = ["https://github.com/grailbio/bazel-compilation-database/archive/0.5.2.tar.gz"],
)

load("@com_grail_bazel_compdb//:deps.bzl", "bazel_compdb_deps")

bazel_compdb_deps()
