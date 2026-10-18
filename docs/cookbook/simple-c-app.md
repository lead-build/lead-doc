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

## Source file types

The `src` list of `lib.lang.c.mod` accepts more than C files. The file type is selected from the file name:

| Pattern | Handled as |
| --- | --- |
| `*.c` | C source, compiled with the C compiler |
| `*.S` | Assembly that is run through the C preprocessor, so it can use `#include` and `#define`. Compiled like C, including dependency tracking of included files |
| `*.s` | Assembly that is not preprocessed. Assembled by the C compiler without dependency tracking, since it cannot include anything |

All of them produce an object file, which is linked into the application the same way, and the same `inc`, `defines` and `cflags` are passed in all cases. So adding an assembly file is just a matter of listing it:

```pbb
lib.lang.c.mod {
    src = [
        "${cwd}/src/main.c",
        "${cwd}/src/startup.S",
        "${cwd}/src/fast_copy.s",
    ];
}
```

## Start and end files

Some targets need objects at a fixed place on the link line, like the `crt1.o`, `crti.o` and `crtn.o` start files that a compiler driver adds when linking a hosted program. For that, `lib.lang.c.mod` has the arguments `start_src` and `end_src`:

```pbb
lib.lang.c.mod {
    start_src = [ "${cwd}/src/crt1.S", "${cwd}/src/crti.S" ];
    src = [ "${cwd}/src/main.c" ];
    end_src = [ "${cwd}/src/crtn.S" ];
}
```

They accept the same file types as `src`, and are compiled the same way. At link time, the objects from `start_src` are placed first on the link line and the objects from `end_src` last, each in the order given. Everything from `src` ends up in between, regardless of which module it came from.

When a module with start or end files is used as a relocated library, they are not relocated into the library object. They are instead placed on the link line of the application that uses the library.

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
