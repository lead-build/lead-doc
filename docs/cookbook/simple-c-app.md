# Building a Simple C Application

The simplest use case: a single C application with a few source files.

**When to use:** Small to medium projects with C code, single target/output.

## Example

```pbb
--8<-- "examples/cookbook/simple-c-app/main.pbb"
```

## How It Works

- `lib.lang.c.app_build "my_app"` — Declares we're building a C application named "my_app"
- `lib.lang.config.simple "${cwd}"` — Sets up basic configuration for the project
- `lib.lang.c.mod` — Defines the C module

## Source Files

The C sources used in this example:

**src/main.c:**
```c
--8<-- "examples/cookbook/simple-c-app/src/main.c"
```

**src/utils.h:**
```c
--8<-- "examples/cookbook/simple-c-app/src/utils.h"
```

**src/utils.c:**
```c
--8<-- "examples/cookbook/simple-c-app/src/utils.c"
```

## Running the Build

Generate and run your build:

```bash
pb
ninja
```

The output binary will be placed in the build directory.

## See Also

- [Multiple Targets with Different Configurations](multiple-targets.md) — For debug/release variants
- [Lead Language](../language/index.md) — To understand the language syntax
