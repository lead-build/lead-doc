# Using Vendored Libraries

Include external libraries in your build.

**When to use:** Incorporating third-party code or internal libraries that have their own build configuration.

## Example

**main.pbb:**
```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" { };
    mylib = include "${cwd}/vendor/mylib/library.pbb" lib;
    
    my_app = lib.merge [
        lib.lang.c.mod {
            src = [ "${cwd}/src/main.c" ];
            inc = [ "${cwd}/src/" ];
        },
        mylib,
    ];
in
lib.build [
    lib.lang.config.simple "${cwd}",
    lib.lang.c.app_build "my_app",
    my_app,
]
```

**vendor/mylib/library.pbb:**
```pbb
|{ cwd, ... }|
| lib |
lib.lang.c.mod {
    src = [
        "${cwd}/src/mylib.c",
    ];
    inc = [
        "${cwd}/src/",
    ];
}
```

## How It Works

- `lib.merge` — Combines your main code with vendored library modules
- `include "${cwd}/vendor/mylib/library.pbb" lib` — Loads and includes another Lead Build file
  - Passes the `lib` object so the vendored library can use the same helpers

The vendored library must expose a module that can be merged into your build.

## Project Structure

```
.
├── main.pbb                    # Your main build configuration
├── src/
│   └── main.c                  # Your code
├── vendor/
│   ├── lead-lib/               # Lead Lib submodule
│   └── mylib/
│       ├── library.pbb         # Library's Lead Build config
│       ├── src/
│       │   ├── mylib.c
│       │   └── mylib.h
│       └── README
└── build/                       # Generated output
```

## Creating a Vendored Library

A vendored library should export modules that can be used via `lib.merge`. Here's an example `vendor/mylib/library.pbb`:

```pbb
|{ cwd, lib, ... }|
let
    this_dir = "${cwd}/vendor/mylib";
in
lib.lang.c.mod {
    src = [
        "${this_dir}/src/mylib.c",
    ];
    inc = [
        "${this_dir}/src/",
    ];
}
```

Your main build then includes it:

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/vendor/lead-lib/lead-lib.pbb" { };
    mylib = include "${cwd}/vendor/mylib/library.pbb" lib;
    
    my_app = lib.merge [
        lib.lang.c.mod { src = [ "${cwd}/src/main.c" ]; },
        mylib,
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
pb -i main.pbb -o build.ninja
ninja -f build.ninja
```

## Advanced: Multiple Vendored Libraries

Combine multiple libraries with your code:

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/vendor/lead-lib/lead-lib.pbb" { };
    lib1 = include "${cwd}/vendor/lib1/library.pbb" lib;
    lib2 = include "${cwd}/vendor/lib2/library.pbb" lib;
    
    my_app = lib.merge [
        lib.lang.c.mod { src = [ "${cwd}/src/main.c" ]; },
        lib1,
        lib2,
    ];
in
lib.build [
    lib.lang.config.simple "${cwd}",
    lib.lang.c.app_build "my_app",
    my_app,
]
```

## Dependencies Between Libraries

If your vendored libraries depend on each other, include them in the right order:

```pbb
my_app = lib.merge [
    lib1,      # Foundation library
    lib2,      # Depends on lib1
    lib.lang.c.mod { src = [ "${cwd}/src/main.c" ]; },
];
```

## See Also

- [Simple C Application](simple-c-app.md) — Start with a simple build first
- [Multiple Targets with Different Configurations](multiple-targets.md) — Combine vendored libraries with multiple builds
- [Modular Build Guide](../under-the-hood/2-modules.md) — Deep dive into modularization
