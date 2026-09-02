# Mixing Languages: C and Rust

Combine Rust and C code in a single project, with C calling Rust libraries.

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

* `lib.merge` — Combines multiple language modules into a single build target
* `lib.lang.c.mod` — The C module with your C code
* `lib.lang.rust.mod` — The Rust module that generates a library

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

## Crate structure

To make Rust code callable from C, use `cbindgen` to build the bindings and
link the library as a `"staticlib"`

**myrustlib/Cargo.toml:**
```toml

[lib]
crate-type = ["staticlib"]

...

[build-dependencies]
cbindgen = "0.24"
```

To generate the header file in the correct location to match the integration,
update `build.rs` based on:

**myrustlib/build.rs:**
```rust
extern crate cbindgen;

use std::{env, path::PathBuf};

fn main() {
    let crate_dir = env::var("CARGO_MANIFEST_DIR").unwrap();

    // CBINDGEN_HEADER_OUTPUT if is relative, should be relative to PWD, not
    // CARGO_MANIFEST_DIR, to match structure of ninja build.
    let pwd = PathBuf::from(env::var("PWD").unwrap());
    let header_name = PathBuf::from(env::var("CBINDGEN_HEADER_OUTPUT").unwrap());
    let header_path = pwd.join(&header_name);
  

    cbindgen::Builder::new()
        .with_crate(&crate_dir)
        .with_config(cbindgen::Config::from_root_or_default(&crate_dir))
        .generate()
        .expect("Unable to generate bindings")
        .write_to_file(&header_path);
}

```

Some extra boilerplate is needed in `lib.rs` to allow for the integration. As a
starting point, this will suffice:

**myrustlib/src/lib.rs:**
```rust
#![no_std]
#![no_main]

#[panic_handler]
fn panic(_info: &core::panic::PanicInfo) -> ! {
    loop {}
}

#[unsafe(no_mangle)]
pub extern "C" fn rust_eh_personality() {}

#[unsafe(no_mangle)]
pub extern "C" fn add_in_rust(a: i32, b: i32) -> i32 {
    a + b
}

```

Rust integration then updates the include path for the c integration, and the
rust library will be available with a header file to include, and access the
exported functions.

**src/main.c:**
```c
#include "myrustlib.h"
#include <stdio.h>

int main() {
    printf("C and Rust Example\n");
    printf("Calling Rust function: add_in_rust(10, 20) = %d\n", add_in_rust(10, 20));
    return 0;
}

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
