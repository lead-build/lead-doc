# Cookbook - Overview

A collection of common build patterns and examples for using Lead in your projects.

## Scenarios

- **[Simple C Application](simple-c-app.md)** — Build a basic C application with source files and include paths
- **[Multiple Targets with Different Configurations](multiple-targets.md)** — Create debug and release builds with different compiler flags
- **[Cross Compilation](cross-compilation.md)** — Build natively and for another platform, with a different compiler and architecture-specific modules
- **[Mixing Languages: C and Rust](c-and-rust.md)** — Combine C and Rust code in a single project
- **[Splitting Your Code Into Libraries](splitting-into-libraries.md)** — Include external or internal libraries in your build
- **[Running Tests](testing.md)** — Build and run tests, with test fixtures and multiple configurations

Each scenario includes a "When to use" section to help you identify which pattern fits your project.

All examples assume that Lead Lib is checked out in the directory `lead-lib/` in the root of the project, as described in [Getting Started](../getting-started.md). The examples are also available as complete projects in the [lead-doc](https://github.com/lead-build/lead-doc) repository, under `examples/cookbook`.
