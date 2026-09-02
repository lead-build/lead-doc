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
lib.tk.flatten [
    # Release build
    lib.build [
        lib.lang.config.simple "${cwd}",
        lib.lang.c.app_build "my_app_release",
        lib.lang.c.config { cflags = [ "-O3" ]; },
        my_app,
    ],

    # Debug build
    lib.build [
        lib.lang.config.simple "${cwd}",
        lib.lang.c.app_build "my_app_debug",
        lib.lang.c.config { cflags = [ "-O0", "-g" ]; },
        my_app,
    ],
]
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
pb -i main.pbb -o build.ninja
ninja -f build.ninja
```

You'll get two executables: `my_app_release` (optimized) and `my_app_debug` (with debug symbols).

## Variations

**Different source files per target:**

```pbb
my_release = lib.lang.c.mod {
    src = [ "${cwd}/src/main.c", "${cwd}/src/release_utils.c" ];
};

my_debug = lib.lang.c.mod {
    src = [ "${cwd}/src/main.c", "${cwd}/src/debug_utils.c" ];
};
```

**Cross-compilation for multiple targets:**

```pbb
lib.tk.flatten [
    lib.build [
        lib.lang.c.config { cc = "arm-linux-gcc"; },
        # ... rest of ARM build
    ],
    lib.build [
        lib.lang.c.config { cc = "x86_64-linux-gcc"; },
        # ... rest of x86 build
    ],
]
```

## See Also

- [Simple C Application](simple-c-app.md) — Start here if you're new
- [Using Vendored Libraries](vendored-libraries.md) — Combine this with libraries
- [Mixing Languages: C and Rust](c-and-rust.md) — Multiple languages with multiple targets
