# Splitting Your Code Into Libraries

Organize your project by splitting code into reusable library modules.

**When to use:** Organizing large projects, isolating functionality, creating reusable components, or keeping application code separate from libraries.

## Example

**main.pbb:**
```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" { };
    mathlib = include "${cwd}/lib/mathlib/mathlib.pbb" lib;
    
    my_app = lib.merge [
        lib.lang.c.mod {
            src = [ "${cwd}/src/main.c" ];
            inc = [ "${cwd}/src/" ];
        },
        mathlib,
    ];
in
lib.build [
    lib.lang.config.simple "${cwd}",
    lib.lang.c.app_build "my_app",
    my_app,
]
```

**lib/mathlib/mathlib.pbb:**
```pbb
|{ cwd, ... }|
| lib |
lib.lang.c.mod {
    src = [
        "${cwd}/src/mathlib.c",
    ];
    inc = [
        "${cwd}/src/",
    ];
}
```

## How It Works

- `lib.merge` — Combines your main application code with library modules
- `include "${cwd}/lib/mathlib/mathlib.pbb" lib` — Loads a library's build configuration from your project
  - Passes the `lib` object so the library module can use the same helpers

Each library module must expose a build module that can be merged into your application.

## Project Structure

```
.
├── main.pbb                    # Main application build
├── src/
│   └── main.c                  # Application code
├── lib/
│   ├── lead-lib/               # Lead Lib submodule
│   └── mathlib/
│       ├── mathlib.pbb         # Library module config
│       └── src/
│           ├── mathlib.c
│           └── mathlib.h
└── build/                       # Generated output
```

## Creating Your Own Library Module

To split your code into libraries, create a `.pbb` file named after your library that exports a module:

**lib/mathlib/mathlib.pbb:**
```pbb
|{ cwd, ... }|
| lib |
lib.lang.c.mod {
    src = [
        "${cwd}/src/mathlib.c",
    ];
    inc = [
        "${cwd}/src/",
    ];
}
```

Then include it in your main build:

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lib/lead-lib/lead-lib.pbb" { };
    mathlib = include "${cwd}/lib/mathlib/mathlib.pbb" lib;
    
    my_app = lib.merge [
        lib.lang.c.mod { src = [ "${cwd}/src/main.c" ]; },
        mathlib,
    ];
in
lib.build [
    lib.lang.config.simple "${cwd}",
    lib.lang.c.app_build "my_app",
    my_app,
]
```

## Running the Build

```bash
pb
ninja
```

## Multiple Libraries

You can split your code into multiple library modules:

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lib/lead-lib/lead-lib.pbb" { };
    mathlib = include "${cwd}/lib/mathlib/mathlib.pbb" lib;
    utillib = include "${cwd}/lib/utillib/utillib.pbb" lib;
    
    my_app = lib.merge [
        lib.lang.c.mod { src = [ "${cwd}/src/main.c" ]; },
        mathlib,
        utillib,
    ];
in
lib.build [
    lib.lang.config.simple "${cwd}",
    lib.lang.c.app_build "my_app",
    my_app,
]
```

## Vendored Libraries

This same pattern also works for external libraries. Instead of `lib/`, place third-party code in `vendor/` and structure it the same way:

```pbb
external_lib = include "${cwd}/vendor/external-lib/external-lib.pbb" lib;
```

The key difference is conceptual: libraries in `lib/` are your own code split into modules, while libraries in `vendor/` are external dependencies. Both follow the naming convention: a library named `mathlib` has its build config in `mathlib/mathlib.pbb`.

## See Also

- [Simple C Application](simple-c-app.md) — Start with a simple build first
- [Multiple Targets with Different Configurations](multiple-targets.md) — Combine vendored libraries with multiple builds
- [Modular Build Guide](../under-the-hood/2-modules.md) — Deep dive into modularization
