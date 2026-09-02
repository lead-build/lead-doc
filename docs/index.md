# Lead Build

Lead Build is a **declarative build system** for expressing build outputs in terms of their dependencies. Instead of scripting commands in sequence, you describe the desired result and how it is composed.

A path value may refer to either a file or a directory, and the system automatically handles the dependencies between them.

## Why Lead Build?

- **Declarative**: Describe *what* to build, not *how* to build it.
- **Modular**: Build logic can be packaged and reused across projects without copying.
- **Pure**: Each build depends on its input parameters only—no global state—enabling multiple variants in a single configuration.
- **Extensible**: Easily combine multiple languages, targets, and configurations without rewriting the build from scratch.

## Key Concepts

### Purity

Lead Build is *pure* in the functional programming sense—with no side effects. This means:

- Each build is a **pure function** of its input parameters
- The same inputs always produce the same outputs
- There is no hidden global state, environment variables that affect behavior, or implicit dependencies
- What you see in your configuration is exactly what you get—nothing hidden or implicit

### Lead Build (the language)
A small, powerful declarative language for describing build outputs. Learn the syntax and concepts in the [Lead Language](language/index.md) guide.

### Lead Lib (the library)
A collection of conventions and helpers that turn the bare language into a practical build system. It provides:

- Standardized build patterns and module interfaces
- Language helpers for C, Rust, and more
- Tools for combining multiple modules and targets
- Support for code generation, cross-compilation, and hierarchical linking

## Example

So what does it look like?

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

Or if you want more, checkout the examples under the cookbook.

## Getting Started

New to Lead Build? Start here:

1. **[Getting Started](getting-started.md)** — Installation and a quick walkthrough
2. **[Cookbook](cookbook/index.md)** — Common build patterns with examples
3. **[Lead Language](language/index.md)** — Learn the language from the ground up

## Going Deeper

Once you're comfortable with the basics:

- **[Language Reference](language-reference/language.md)** — Complete syntax and built-in functions
- **[Lead Lib Internals](under-the-hood/index.md)** — Advanced patterns: multiple targets, configuration, and modularization

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
