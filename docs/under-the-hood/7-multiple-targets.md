# Pill 7 - Multiple targets

We have shown how lead-lib can be used to generate flexible targets. But we
still need to show how it can be used to run multiple configurations in
parallel.

Since `lib.build` is pure, meaning it only generates what is passed as input and
returns the build as a return value, it is perfectly valid to run it several
times within the same build script.

## Release vs. debug build

In this example, we can show how to use the same modules defining source—or
even defining code generation—by passing them to multiple `lib.build`
instances.

This can be used when multiple targets, architectures, or similar variations are
needed for the same source.

In this case, we are going to do two separate builds: one with `-O3` and one
with `-O0` and `-g`, simply by switching the configuration module while
preserving the rest.

Only update `main.pbb` from the previous pill to:

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" {
        languages = {
            sp = include "${cwd}/lang_structpack.pbb";
        };
    };

    target_release = | _ |
    {
        config = {
            objbasedir = "${cwd}/build/release";
            srcdirs = [ "${cwd}" ];
            outdir = "${cwd}/out_release";
        };
        c = {
            cflags = [ "-O3" ];
        };
    };

    target_debug = | _ |
    {
        config = {
            objbasedir = "${cwd}/build/debug";
            srcdirs = [ "${cwd}" ];
            outdir = "${cwd}/out_debug";
        };
        c = {
            cflags = [
                "-O0",
                "-g",
                "-ggdb"
            ];
        };
    };

     # lead-lib uses a wrapper to make the code cleaner. It's just a normal
     # module as seen before.
    my_app = lib.lang.c.mod {
        src = [
            "${cwd}/main.c",
            "${cwd}/model/model.c",
            "${cwd}/model/some_other_model.c",
        ];
        inc = [
            "${cwd}/model",
        ];
    };
    my_sp_mod = lib.lang.sp.mod {
        inc = [
            "${cwd}/model",
        ];
        src = [
            "${cwd}/model/some_struct.toml",
        ];
    };

    app = lib.merge [
        my_app,
        my_sp_mod,
    ];
in
 # lead-build can handle a list of generated outputs. However, lib.build already
 # returns a list, so flatten it using the lead-lib toolkit function "flatten".
lib.tk.flatten [
    lib.build [
        lib.lang.c.app_build "my_app",
        target_release,
        app,
    ],
    lib.build [
        lib.lang.c.app_build "my_app_debug",
        target_debug,
        app,
    ],
]
```

This shows that two different `lib.build` calls are made, with different
targets and different build directories.

## Getting other outputs than the build

`lib.build` always returns the field `common.out` of the resolved modules, which is the main way of describing what a project builds, and what `lib.lang.c.app_build` sets. But sometimes another part of the project is of interest, such as only the object files, or a path from the configuration.

For that, there is `lib.combine`. It takes the same list of modules as `lib.build`, merges them together with the language defaults, and resolves all the dependent variables as described in pill 4, but returns the whole resolved module instead of just `common.out`. In fact, `lib.build` is defined as:

```pbb
build = |mods| (combine mods).common.out;
```

Any field in the result can then be used, and as usual, only the fields that are referenced are evaluated:

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" { };

    mods = [
        lib.lang.c.app_build "my_app",
        lib.lang.config.simple "${cwd}",
        lib.lang.c.mod {
            src = [ "${cwd}/src/main.c" ];
            inc = [ "${cwd}/src/" ];
        },
    ];

    combined = lib.combine mods;
in
    # Build only the object files, not the linked application
    combined.c.obj
```

Combining the result of `lib.combine` with other modules is not needed; add the modules to the list before calling it.

## Running tests for multiple targets

Tests can be built and run for several targets in the same way, by passing one list of modules per target to `lib.test_multi_report`. See the cookbook entry [Running Tests](../cookbook/testing.md).
