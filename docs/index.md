# Lead Build

Lead Build is a declarative build system for expressing build outputs in terms of their dependencies. A path value may refer to either a file or a directory. Instead of scripting a sequence of commands, Lead Build describes the desired result and how it is composed.

## Why Lead Build

- Declarative: describe *what* to build, not *how* to build it.
- Modular: build logic can be packaged and reused across projects.
- Reusable: common build patterns can be shared without duplicating file paths or command sequences.

## Example

So how does it look like then?

All examples can be seen in the [git repository](https://github.com/lead-build/lead-doc).

```pbb
|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" { };
in
lib.build [
    lib.lang.c.app_build "hello_world",
    lib.lang.config.simple "${cwd}",
    lib.lang.c.mod {
        src = [
            "${cwd}/src/main.c",
            "${cwd}/src/lib.c",
        ];
        inc = [
            "${cwd}/src/"
        ];
    },
]
```

Note some parts: The build is generic, it is *configured* to contain C and a C
output. And it has no globals.

That means it can be extensible. So what happens if it grows?

```pbb

|{ cwd, include, ... }|
let
    lib = include "${cwd}/lead-lib/lead-lib.pbb" { };

    my_app = lib.merge [
        lib.lang.c.mod {
            src = [
                "${cwd}/src/main.c",
            ];
        },

         # Maybe some rust with your C?
        lib.lang.rust.mod {
            name = "myrustlib";
            dir = "${cwd}/myrustlib";
        },

         # Maybe some external or internal library with your code?
        include "${cwd}/vendor/library.pbb" lib,
    ];
in
lib.tk.flatten [

     # Build for release with optimization
    lib.build [
        lib.lang.config.simple "${cwd}",
        lib.lang.c.app_build "hello_world",
        lib.lang.c.config {
            cflags = [ "-O3" ];
        },
        my_app,
    ],

     # Build for debug with no optimization and debug symbols

    lib.build [
        lib.lang.config.simple "${cwd}",
        lib.lang.c.app_build "hello_debug",
        lib.lang.c.config {
            cflags = [
                "-O0",
                "-g"
            ];
        },
        my_app,
    ],
]
```

Shows how it can easily be extended to simply include vendored modules, do
parallell builds with different build flags and targets.


## Installation

Currently available as a Rust crate.

Run:
```
cargo install lead-build
```

Or check out the git repository at [https://github.com/lead-build/lead-build](https://github.com/lead-build/lead-build)

Then checkout `https://github.com/lead-build/lead-lib.git` as a submodule to
your project.

## Comparison with other build systems

The classic `make` tool is initially easy and powerful for:

- having a single target binary
- a set of source files
- a global set of compiler flags

This is often true for smaller projects that compile natively. For example:

```make
APP=my_app

SRCS=\
    src/main.c \
    src/mylib.c

OBJS=$(patsubst src/%.c,obj/%.o,$(SRCS))

obj/%.o: src/%.c
    @mkdir -p $(@D)
    gcc -c -o $@ $<

$(APP): $(OBJS)
    gcc -o $@ $^
```

However, what happens if you also want to:

- compile one variant for debugging with `-O0 -g`
- compile another variant for release with `-O3`
- compile tests with different libraries
- generate source files such as protocol buffers or parser grammars

A bigger issue appears when writing for embedded systems and multiple targets:

- compiler changes between targets, but source files are *mostly* the same
- libraries for the same architecture can be reused, but
    - linking may differ depending on memory architecture
    - different board support packages may be required
- digital twins and simulators may use totally different compilers

Any build system that relies on global state - for example, one that assumes a global list of source files - becomes problematic when the set of inputs is target-dependent.

This is why declarative builds matter: each build is *pure* - it depends on its input parameters and *only* its input parameters, even if it at first glance looks a bit more complicated.

## Lead build vs. lead lib

[lead-build](https://lead-build.readthedocs.io) is a declarative language for
describing build projects. It enables reusable modules that are architecture-
and compiler-independent, so integrators can choose what to include without
adopting a library's internal structure.

This requires conventions and libraries for specifying builds, so each project
implements the same interface and modules compose cleanly.

This is where *lead-lib* comes in.

At a high level, lead-lib provides:

- Conventions for how modules integrate (the build API)
- Tools for implementing boilerplate module builds

It does so while remaining:
- language independent
- naturally supportive of code generation, such as parser generators and
  protocol generators
- capable of hierarchical linking, for example by combining reusable libraries
  into one build output that can be used as input to the next
- compatible with multiple architectures in the same build, for example with
  non-global CFLAGS

It also provides language and toolkit helpers.

Implemented language packages:
- `common` for shared target metadata helpers
- `c` for gcc-based C compilation and linking

Toolkit helpers:
- `tk.flatten` for flattening list-of-lists values used in build graph
  composition
