# Mixing Languages: C and Rust

Combine Rust and C code in a single project, with C calling Rust libraries.

**When to use:** When part of a C application is written in Rust, or when gradually introducing Rust into an existing C project.

## Example

```pbb
--8<-- "examples/cookbook/c-and-rust/main.pbb"
```

## How It Works

* `lib.merge` — Combines multiple language modules into a single build target
* `lib.lang.c.mod` — The C module with your C code
* `lib.lang.rust.mod` — The Rust module that generates a library

When compiled, Lead will:

1. Build the Rust crate first (generates a C-compatible library header and binary)
2. Compile the C code with the Rust library available
3. Link everything together

## Project Structure

Typical directory layout:

```
.
├── main.pbb                    # Lead configuration
├── src/
│   └── main.c                  # C code that calls Rust
├── myrustlib/                  # Rust crate
│   ├── Cargo.toml
│   ├── build.rs                # Generates C header from Rust
│   ├── cbindgen.toml           # Configuration of the header generation
│   └── src/
│       └── lib.rs
└── lead-lib/                   # Lead Lib submodule
```

## Crate structure

To make Rust code callable from C, use `cbindgen` to build the bindings and
link the library as a `"staticlib"`.

**myrustlib/Cargo.toml:**
```toml
--8<-- "examples/cookbook/c-and-rust/myrustlib/Cargo.toml"
```

To generate the header file in the correct location to match the integration,
update `build.rs` based on:

**myrustlib/build.rs:**
```rust
--8<-- "examples/cookbook/c-and-rust/myrustlib/build.rs"
```

Some extra boilerplate is needed in `lib.rs` to allow for the integration. As a
starting point, this will suffice:

**myrustlib/src/lib.rs:**
```rust
--8<-- "examples/cookbook/c-and-rust/myrustlib/src/lib.rs"
```

The Rust integration then updates the include path for the C integration, so
the Rust library is available as a header file to include, giving access to
the exported functions.

**src/main.c:**
```c
--8<-- "examples/cookbook/c-and-rust/src/main.c"
```

The header generation is configured in `cbindgen.toml`. The important part is
that the language is set to C:

**myrustlib/cbindgen.toml:**
```toml
--8<-- "examples/cookbook/c-and-rust/myrustlib/cbindgen.toml"
```

## Running the Build

```bash
pb
ninja
```

## Advanced: Multiple Rust Libraries

```pbb
my_app = lib.merge [
    lib.lang.c.mod { src = [ "${cwd}/src/main.c" ]; },
    lib.lang.rust.mod {
        name = "rustlib1";
        dir = "${cwd}/rustlib1";
    },
    lib.lang.rust.mod {
        name = "rustlib2";
        dir = "${cwd}/rustlib2";
    },
];
```

## See Also

- [Simple C Application](simple-c-app.md) — Basic C building
- [Multiple Targets with Different Configurations](multiple-targets.md) — Multiple builds with this pattern
- [Modular Build Guide](../under-the-hood/2-modules.md) — Deep dive into modules
