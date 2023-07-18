load("@bazel_tools//tools/build_defs/cc:action_names.bzl", "ACTION_NAMES")
load(
    "@bazel_tools//tools/cpp:cc_toolchain_config_lib.bzl",
    "feature",
    "flag_group",
    "flag_set",
    "tool_path",
)

all_compiler_actions = [
    ACTION_NAMES.cc_flags_make_variable,
    ACTION_NAMES.c_compile,
    ACTION_NAMES.cpp_compile,
    ACTION_NAMES.cpp_header_parsing,
]

all_link_actions = [
    ACTION_NAMES.cpp_link_executable,
    ACTION_NAMES.cpp_link_dynamic_library,
    ACTION_NAMES.cpp_link_nodeps_dynamic_library,
]

def _impl(ctx):
    tool_paths = [
        tool_path(name = "gcc", path = "/opt/avr-toolchain/bin/avr-gcc"),
        tool_path(name = "ar", path = "/opt/avr-toolchain/bin/avr-ar"),
        tool_path(name = "cpp", path = "/opt/avr-toolchain/bin/avr-cpp"),
        tool_path(name = "ld", path = "/opt/avr-toolchain/bin/avr-ld"),
        tool_path(name = "nm", path = "/opt/avr-toolchain/bin/avr-nm"),
        tool_path(name = "objdump", path = "/opt/avr-toolchain/bin/avr-objdump"),
        tool_path(name = "objcopy", path = "/opt/avr-toolchain/bin/avr-objcopy"),
        tool_path(name = "strip", path = "/opt/avr-toolchain/avr/bin/strip"),
    ]
    features = [
        feature(
            name = "opt",
            flag_sets = [
                flag_set(
                    actions = all_compiler_actions,
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-DNDEBUG",
                            ],
                        ),
                    ]),
                ),
            ],
        ),
        feature(
            name = "dbg",
            flag_sets = [
                flag_set(
                    actions = all_compiler_actions + all_link_actions,
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-gdwarf-2",
                                "-ggdb3",
                            ],
                        ),
                    ]),
                ),
                flag_set(
                    actions = all_compiler_actions,
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-DDEBUG",
                            ],
                        ),
                    ]),
                ),
            ],
        ),
        feature(
            name = "defaults",
            enabled = True,
            flag_sets = [
                flag_set(
                    actions = [ACTION_NAMES.cpp_link_executable],
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-lm",
                                "-Wl,--relax",
                                "-Wl,--gc-sections",
                            ],
                        ),
                    ]),
                ),
                flag_set(
                    actions = [ACTION_NAMES.cpp_link_static_library],
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "--plugin",
                                "/opt/avr-toolchain/libexec/gcc/avr/13.1.0/liblto_plugin.so",
                            ],
                        ),
                    ]),
                ),
                flag_set(
                    actions = [ACTION_NAMES.c_compile],
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-std=c2x",
                            ],
                        ),
                    ]),
                ),
                flag_set(
                    actions = [ACTION_NAMES.cpp_compile],
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-std=c++20",
                            ],
                        ),
                    ]),
                ),
                flag_set(
                    actions = all_compiler_actions,
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-Wall",
                                "-Werror",
                                "-Wextra",
                                "-DF_CPU=" + ctx.attr.freq,
                                "--param=min-pagesize=0",
                                "-O2",
                                "-I/opt/avr-toolchain/include",
                                "-I/opt/avr-toolchain/avr/include",
                                "-I/usr/local/include/simavr",
                            ],
                        ),
                    ]),
                ),
                flag_set(
                    actions = all_compiler_actions + all_link_actions,
                    flag_groups = ([
                        flag_group(
                            flags = [
                                "-Wl,--undefined=_mmcu,--section-start=.mmcu=0x910000",
                                "-mmcu=" + ctx.attr.mmcu,
                                "-ffunction-sections",
                                "-fdata-sections",
                                "-fno-fat-lto-objects",
                                "-flto",
                            ],
                        ),
                    ]),
                ),
            ],
        ),
    ]
    return cc_common.create_cc_toolchain_config_info(
        cxx_builtin_include_directories = [
            "/opt/avr-toolchain/include",
            "/opt/avr-toolchain/avr/include",
            "/opt/avr-toolchain/lib/gcc/avr/13.1.0/include",
            "/usr/local/include/simavr",
        ],
        abi_libc_version = "avr",
        abi_version = "avr",
        compiler = "avr-gcc",
        ctx = ctx,
        features = features,
        host_system_name = "local",
        target_libc = "avr",
        target_cpu = "avr",
        target_system_name = "avr",
        toolchain_identifier = "avr-toolchain",
        tool_paths = tool_paths,
    )

cc_toolchain_config = rule(
    attrs = {
        "mmcu": attr.string(mandatory = True),
        "freq": attr.string(mandatory = True),
    },
    provides = [CcToolchainConfigInfo],
    implementation = _impl,
)
