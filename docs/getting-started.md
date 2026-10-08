# Getting Started

## Installation

The Lead tools are available as the Rust crate `lead-build`. To run the builds that Lead generates, you also need [Ninja](https://ninja-build.org/).

### From Cargo

Run:
```bash
cargo install lead-build
```

This installs the `pb` command, together with the formatter `pbfmt` and the language server `pbls`. See [Tooling](tooling/index.md) for more about them.

### From Source

Check out the git repository at [https://github.com/lead-build/lead-build](https://github.com/lead-build/lead-build), and run `cargo install --path .` in it.

### Setting up Lead Lib

Lead Lib provides reusable build patterns and language helpers. Check out the repository as a submodule in your project:

```bash
git submodule add https://github.com/lead-build/lead-lib.git lead-lib
```

## Quick Start

Here's a minimal example of a Lead project, saved as `main.pbb` in the root of the project:

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" { };
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

- `include` brings in Lead Lib
- `lib.build` creates the actual build configuration
- `lib.lang.c.app_build "my_app"` declares we're building a C application named "my_app"
- `lib.lang.config.simple` sets up basic configuration
- `lib.lang.c.mod` defines the C module with source files and include paths

### Running a Build

To generate a Ninja build file from `main.pbb` and run the build:

```bash
pb
ninja
```

## Next Steps

- Explore the [Cookbook](cookbook/index.md) for common build patterns
- Learn the [Lead Language](language/index.md) to understand the core concepts
- Dive into [Lead Lib internals](under-the-hood/index.md) for advanced usage
