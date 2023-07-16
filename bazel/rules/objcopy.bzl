load("@bazel_tools//tools/cpp:toolchain_utils.bzl", "find_cpp_toolchain", "use_cpp_toolchain")

def _objcopy_impl(ctx):
    src = ctx.attr.src.files.to_list()[0]
    binary = ctx.actions.declare_file(src.basename + ".hex")
    toolchain = find_cpp_toolchain(ctx)

    args = ctx.actions.args()
    args.add("-O", "ihex")
    args.add("-j", ".text")
    args.add("-j", ".data")
    args.add("-j", ".eeprom")
    args.add(src)
    args.add(binary)

    ctx.actions.run(
        executable = toolchain.objcopy_executable,
        outputs = [binary],
        inputs = depset(
            direct = [src],
            transitive = [
                toolchain.all_files,
            ],
        ),
        arguments = [args],
        use_default_shell_env = True,
    )

    return [
        DefaultInfo(
            files = depset([binary]),
        ),
    ]

objcopy = rule(
    implementation = _objcopy_impl,
    attrs = {
        "src": attr.label(allow_single_file = True, mandatory = True),
    },
    toolchains = use_cpp_toolchain(),
    executable = False,
)
