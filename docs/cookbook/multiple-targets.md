# Building Multiple Targets with Different Configurations

Create different binaries with different compiler flags (e.g., debug vs. release).

**When to use:** When you need multiple build variants like debug/release, different optimization levels, or target-specific builds.

## Example

```pbb
--8<-- "examples/cookbook/multiple-targets/main.pbb"
```

## How It Works

The key pattern here is:

1. **Define the shared module once** — `my_app` contains the common C code and include paths
2. **Create multiple build targets** — Each `lib.build` creates a separate output with different configuration
3. **Apply variant-specific settings** — Use `lib.lang.c.config` to set different compiler flags for each target
4. **Name the builds** — The file evaluates to an object with one field per build

This approach avoids duplicating source file lists while allowing each target to have its own compiler flags and output name.

## Source Files

The same source file is used for both builds:

**src/main.c:**
```c
--8<-- "examples/cookbook/multiple-targets/src/main.c"
```

## Running the Build

Generate and run your build:

```bash
pb
ninja
```

You'll get two executables: `my_app_release` (optimized) and `my_app_debug` (with debug symbols).

## Aliases

Letting the output be an object of builds means the field names are available as aliases in Ninja. Building only the release variant is done by running `ninja release`, and only the debug variant by `ninja debug`.

```pbb
--8<-- "examples/cookbook/multiple-targets/main.pbb:12:28"
```

## See Also

- [Simple C Application](simple-c-app.md) — Start here if you're new
- [Splitting Into Libraries](splitting-into-libraries.md) — Combine this with libraries
- [Mixing Languages: C and Rust](c-and-rust.md) — Multiple languages with multiple targets
