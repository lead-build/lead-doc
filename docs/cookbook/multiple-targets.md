# Building Multiple Targets with Different Configurations

Create different binaries with different compiler flags (e.g., debug vs. release).

**When to use:** When you need multiple build variants like debug/release, different optimization levels, or target-specific builds.

## Example

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" { };
    
    my_app = lib.lang.c.mod {
        src = [
            "${cwd}/src/main.c",
        ];
        inc = [ "${cwd}/src/" ];
    };
in
{
    # Release build
    release = lib.build [
        lib.lang.config.simple "${cwd}",
        lib.lang.c.app_build "my_app_release",
        lib.lang.c.config { cflags = [ "-O3" ]; },
        my_app,
    ];

    # Debug build
    debug = lib.build [
        lib.lang.config.simple "${cwd}",
        lib.lang.c.app_build "my_app_debug",
        lib.lang.c.config { cflags = [ "-O0", "-g" ]; },
        my_app,
    ];
}
```

## How It Works

The key pattern here is:

1. **Define the shared module once** — `my_app` contains the common C code and include paths
2. **Create multiple build targets** — Each `lib.build` creates a separate output with different configuration
3. **Apply variant-specific settings** — Use `lib.lang.c.config` to set different compiler flags for each target
4. **Flatten the result** — `lib.tk.flatten` combines the list of builds into a single output structure

This approach avoids duplicating source file lists while allowing each target to have its own compiler flags and output name.

## Running the Build

Generate and run your build:

```bash
pb
ninja
```

You'll get two executables: `my_app_release` (optimized) and `my_app_debug` (with debug symbols).

## Aliases

Letting the output be an object of builds means the output will be availalbe as
alises in ninja. Building only `release` is done by calling `ninja release`

```pbb
{
    # Release build
    release = lib.build [
        lib.lang.config.simple "${cwd}",
        lib.lang.c.app_build "my_app_release",
        lib.lang.c.config { cflags = [ "-O3" ]; },
        my_app,
    ];

    # Debug build
    debug = lib.build [
        lib.lang.config.simple "${cwd}",
        lib.lang.c.app_build "my_app_debug",
        lib.lang.c.config { cflags = [ "-O0", "-g" ]; },
        my_app,
    ];
}
```

## See Also

- [Simple C Application](simple-c-app.md) — Start here if you're new
- [Splitting Into Libraries](splitting-into-libraries.md) — Combine this with libraries
- [Mixing Languages: C and Rust](c-and-rust.md) — Multiple languages with multiple targets
