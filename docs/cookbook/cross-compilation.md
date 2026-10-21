# Cross Compilation

Build the same application both natively for the host and cross compiled for a platform, using a different compiler and architecture-specific modules for each.

**When to use:** When the application runs on a platform other than the host, such as an embedded board, but you also want a native build to run and debug on the host.

## Example

```pbb
--8<-- "examples/cookbook/cross-compilation/main.pbb"
```

## How It Works

This builds on [Multiple Targets](multiple-targets.md), with two differences:

1. **The platform target changes the compiler** — `lib.lang.c.config` sets `cmd_cc` and `cmd_ld` to `clang`, and passes the target architecture flags in both `cflags` and `ldflags`
2. **Some modules are architecture-specific** — The board support package (BSP) has a real implementation in `bsp_riscv64.c` for the platform, and a mock in `bsp_mock.c` that runs on the host

Everything that differs between the targets is collected in the `board` object, with one module per board. Each one merges the compiler configuration with the BSP for that board. The application, `my_app`, is shared, and only uses the BSP interface in `bsp.h`. Selecting a target is then a matter of picking a board when combining the modules for the output.

The board also sets the architecture with `lib.lang.config.mod { arch = "riscv64"; }`. The native board keeps the lead-lib default, `posix`. The AES library uses it to select its implementation, as described [below](#selecting-modules-by-architecture).

Each build compiles all sources with its own compiler and flags, into its own build directory:

```ninja
build build/my_app_native/bsp/bsp_mock.o: clang_c_o_MMD bsp/bsp_mock.c
build build/my_app_native/lib/aes/src/aes_sw.o: clang_c_o_MMD lib/aes/src/aes_sw.c
build build/my_app_native/src/main.o: clang_c_o_MMD src/main.c
build build/my_app_platform/bsp/bsp_riscv64.o: clang_c_o_MMD1 bsp/bsp_riscv64.c
build build/my_app_platform/lib/aes/src/aes_hw_riscv64.o: clang_c_o_MMD1 lib/aes/src/aes_hw_riscv64.c
build build/my_app_platform/src/main.o: clang_c_o_MMD1 src/main.c
build out/my_app_native: clang_o build/my_app_native/bsp/bsp_mock.o build/my_app_native/src/main.o build/my_app_native/lib/aes/src/aes_sw.o
build out/my_app_platform: clang_o_targetriscvunknownelf build/my_app_platform/bsp/bsp_riscv64.o build/my_app_platform/src/main.o build/my_app_platform/lib/aes/src/aes_hw_riscv64.o
```

The platform target in this example is only there to prove the point. The platform build is linked with `-nostdlib`, since there is no C library for the platform, and has no start code, so the linker warns that it can't find `_start`. A real platform would provide its start code, for example with `start_src` as described in [Simple C Application](simple-c-app.md#start-and-end-files), and a linker script in `ldflags`.

## Source Files

The C sources are not shown here, since they don't affect the build files. The application only uses the interfaces of the BSP and the AES library, and each has one implementation for the host and one for the platform. The complete project is available in the [lead-doc](https://github.com/lead-build/lead-doc) repository, under `examples/cookbook/cross-compilation`.

## Running the Build

Generate the build, and build each target through its alias:

```bash
pb
ninja native
ninja platform
```

You'll get two executables: `out/my_app_native`, which runs on the host, and `out/my_app_platform`, a RISC-V executable for the platform.

## Selecting Modules by Architecture

The BSP is picked by the board at the top level. But sometimes a module further down in the application tree has architecture-specific parts, and the top level shouldn't need to know about them. Here, the AES library has a driver for the hardware accelerator of the platform, and a software implementation for the host.

The library selects the implementation by itself, using `lib.gate`. It takes a condition and a module, and only includes the module when the condition is true for the resolved build. Here, the condition is on `config.arch`, which is set by the board:

**lib/aes/aes.pbb:**
```pbb
--8<-- "examples/cookbook/cross-compilation/lib/aes/aes.pbb"
```

The application just includes the library, and merges it into `my_app`, the same way for both targets:

```pbb
--8<-- "examples/cookbook/cross-compilation/main.pbb:5:15"
```

Since the condition is evaluated on the resolved build, it is the board passed to `lib.build` that decides which implementation is used, no matter how deep in the tree the library is.

## See Also

- [Multiple Targets with Different Configurations](multiple-targets.md) — The basis for this pattern
- [Running Tests](testing.md) — The native target, with the mock BSP, is a good place to run tests
