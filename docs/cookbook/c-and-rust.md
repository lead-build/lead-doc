# Mixing Languages: C and Rust

Combine Rust and C code in a single project, with C calling Rust libraries.

**When to use:** Leveraging Rust for performance-critical or safety-critical components while keeping existing C code.

## Example

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" { };
    
    my_app = lib.merge [
        lib.lang.c.mod {
            src = [ "${cwd}/src/main.c" ];
            inc = [ "${cwd}/src/" ];
        },
        lib.lang.rust.mod {
            name = "myrustlib";
            dir = "${cwd}/myrustlib";
        },
    ];
in
lib.build [
    lib.lang.config.simple "${cwd}",
    lib.lang.c.app_build "my_app",
    my_app,
]
```

## How It Works

- `lib.merge` — Combines multiple language modules into a single build target
- `lib.lang.c.mod` — The C module with your C code
- `lib.lang.rust.mod` — The Rust module that generates a library
  - `name` — The Rust crate name
  - `dir` — Path to the Rust project (containing `Cargo.toml`)

When compiled, Lead Build will:

1. Build the Rust crate first (generates a C-compatible library header and binary)
2. Compile the C code with the Rust library available
3. Link everything together

## Project Structure

Typical directory layout:

```
.
├── main.pbb                    # Lead Build configuration
├── src/
│   └── main.c                  # C code that calls Rust
├── myrustlib/                  # Rust crate
│   ├── Cargo.toml
│   ├── cbindgen.toml          # Generates C header from Rust
│   └── src/
│       └── lib.rs
└── vendor/
    └── lead-lib/               # Lead Lib submodule
```

## Using cbindgen

To make Rust code callable from C, use `cbindgen`:

**myrustlib/Cargo.toml:**
```toml
[build-dependencies]
cbindgen = "0.24"
```

**myrustlib/build.rs:**
```rust
use cbindgen::Language;

fn main() {
    cbindgen::generate(".")
        .expect("cbindgen failed")
        .write_to_file("target/myrustlib.h");
}
```

**myrustlib/src/lib.rs:**
```rust
#[no_mangle]
pub extern "C" fn my_function(x: i32) -> i32 {
    x * 2
}
```

**src/main.c:**
```c
#include "../myrustlib/target/myrustlib.h"

int main() {
    int result = my_function(21);
    printf("Result: %d\n", result);
    return 0;
}
```

## Running the Build

```bash
pb -i main.pbb -o build.ninja
ninja -f build.ninja
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
