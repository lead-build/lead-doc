# Lead Build

Lead Build is a declarative build system for expressing build outputs in terms of their dependencies. A path value may refer to either a file or a directory. Instead of scripting a sequence of commands, Lead Build describes the desired result and how it is composed.

## Why Lead Build

- Declarative: describe *what* to build, not *how* to build it.
- Modular: build logic can be packaged and reused across projects.
- Reusable: common build patterns can be shared without duplicating file paths or command sequences.

## Example

And a small example of how a lead-build can look:

(TODO: example using lead-lib)

```lead
|{include, cwd, pb, ...}|
let
    leadlib = include cwd / "lead-lib" / "main.pbb";
    my_lib = include cwd / "mylib" / "main.pbb";
in
leadlib.lang.c.build {
    output = cwd / "myapp";
    builddir = cwd / "build";

    sources = [
        cwd / "src" / "main.c",
        cwd / "src" / "mylib.c",
    ] ++ my_lib.sources;

    includes = [
        cwd / "src";
    ] ++ my_lib.includes;
}
```

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

### Reusability

Using a declarative language to define builds also enables reuse.

Imagine a library, for example something small - an embedded implementation of `printf` - or something bigger - an IPv6 network stack.

For the integrator, you want to:

1. download the library, possibly using git submodules
2. add it to the build system, making:
    - its headers available in the include path
    - sources added to the build, possibly via intermediate `.o` files
    - compilation use the correct per-target flags
    - and not worry about its internal structure beyond the public API
3. add it to the targets you want to include, but possibly not all of them
4. build

This is possible if the library specifies its build definition in *lead-build* format and uses conventions for exposing the build.

Since the build format of the library is defined by the library itself, the library can be upgraded internally without changing how it is integrated, as long as its public API stays compatible.

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

## Usage

The best way to use lead-lib is to check it out as a submodule within your
project and `include` the file `lead-lib.pbb` in your project's `main.pbb`.

That makes sure your build stays consistent until you manually upgrade the
library.

## File structure

For users, there are two main file types: project files and module files.

The project file is placed at the top level of the project and is called
`main.pbb`. It specifies the output of the deliverable from the project,
including linking and packaging.

The module file is placed in a module folder. By convention, it has the same
name as the module, with a `.pbb` suffix. It contains the module's output,
intended for further processing within a parent lead-build script.

## Theory of operation

The build system focuses on what should be delivered, not on source layout.

Given target parameters, each module provides a list of *deliverables* from the
module's point of view.

For example, a module containing an application's source code delivers the
*object* files needed to build the application, based on the *environment*
containing the compiler and its parameters.

The module's `out` function then performs the final link step.

### Module file structure

Each module must be able to provide build input to other modules using system
configuration input. Therefore, modules are loaded in three passes:

1. Environment and target configuration
2. Object generation (compilation)
3. Output generation (linking)

From an external perspective, the minimal module file is:

```pbb
|{...}|
|{...} @ config|
{
    target = {
        c = {
            inc = [
                cwd
            ];
        };
    };

    obj = |{c = {cc, ...}}| [
        cc "${cwd}/myfile_a.c",
        cc "${cwd}/myfile_b.c",
        cc "${cwd}/myfile_c.c",
    ];

    out = |{c = {ld, ...}, ...} obj| [
        ld "${cwd}/build/output.elf" obj
    ];
}
```

where `obj` represents pass 2 (compilation) and `out` represents pass 3 (linking).

The `out` function receives the build environment and the list of object files
produced by `obj`, and returns the final build outputs (e.g. linked ELF files).
This is where the link step is performed, combining all object files into the
deliverable binary.

### Libraries and intermediate builds

When building libraries that do not depend on external sources, the library can
be defined as a build in pass 1 and then returned in pass 2. This leverages
laziness in lead-lang for reuse across multiple targets, so the library is
built only once.

The requirement is that pass 2 returns _a_ correct build in the `obj` field,
along with information in the language-specific environment that other modules
can access.

### Top-level exports

Including `lead-lib.pbb` provides these top-level entries:

- `build` for module build orchestration
- `out_files` for selecting output artifacts
- `merge` for combining module outputs
- `tk` for common utility helpers
- `lang` for language package access

### Shared target metadata helpers

The `lang.common` package includes shared helpers that can be used across
language backends.

- `merge_target`, which merges target-level metadata and appends
  `common.subdir` values from multiple target fragments.

This is used by the C backend so object paths can be translated to target-
specific output subdirectories. In practice, object output now resolves under:

`config.common.objdir / target.common.subdir`
