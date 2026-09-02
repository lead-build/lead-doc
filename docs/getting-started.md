# Getting Started

## Installation

Lead Build is available as a Rust crate.

### From Cargo

Run:
```bash
cargo install lead-build
```

### From Source

Check out the git repository at [https://github.com/lead-build/lead-build](https://github.com/lead-build/lead-build)

### Setting up Lead Lib

Lead Lib provides reusable build patterns and language helpers. Checkout the repository as a submodule to your project:

```bash
git submodule add https://github.com/lead-build/lead-lib.git vendor/lead-lib
```

## Quick Start

Here's a minimal example of a Lead Build project:

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/vendor/lead-lib/lead-lib.pbb" { };
in
lib.build [
    lib.lang.c.app_build "my_app",
    lib.lang.config.simple "${cwd}",
    lib.lang.c.mod {
        src = [
            "${cwd}/src/main.c",
        ];
        inc = [
            "${cwd}/src/"
        ];
    },
]
```

This creates a simple C application build. Let's break down what's happening:

- `include` brings in the lead-lib library
- `lib.build` creates the actual build configuration
- `lib.lang.c.app_build "my_app"` declares we're building a C application named "my_app"
- `lib.lang.config.simple` sets up basic configuration
- `lib.lang.c.mod` defines the C module with source files and include paths

### Running a Build

To generate a Ninja build file and run the build:

```bash
pb -i main.pbb -o build.ninja
ninja -f build.ninja
```

To evaluate the build file without generating output (useful for debugging):

```bash
pb -E -i main.pbb
```

## Next Steps

- Learn the [Lead Build Language](language/index.md) to understand the core concepts
- Explore the [Cookbook](cookbook/index.md) for common build patterns
- Dive into [Lead Lib internals](under-the-hood/index.md) for advanced usage
