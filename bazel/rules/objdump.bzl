"""
objdump.bzl - Bazel rules for objdump
"""

load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain", "use_cpp_toolchain")

def _objdump_impl(ctx):
    src = ctx.attr.src.files.to_list()[0]
    binary = ctx.actions.declare_file(src.basename + ".s")
    toolchain = find_cpp_toolchain(ctx)

    args = ctx.actions.args()
    args.add("-S")
    args.add("-j", ".text")
    args.add("-j", ".data")
    args.add("-j", ".eeprom")
    args.add("-j", ".mmcu")
    args.add("-d")
    args.add(src)

    ctx.actions.run_shell(
        outputs = [binary],
        inputs = depset(
            direct = [src],
            transitive = [
                toolchain.all_files,
            ],
        ),
        command = toolchain.objdump_executable + " $@ > " + binary.path,
        arguments = [args],
        use_default_shell_env = True,
    )

    return [
        DefaultInfo(
            files = depset([binary]),
        ),
    ]

objdump = rule(
    implementation = _objdump_impl,
    attrs = {
        "src": attr.label(allow_single_file = True, mandatory = True),
    },
    toolchains = use_cpp_toolchain(),
    executable = False,
)
