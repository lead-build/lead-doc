# Lead Build System

The Lead Build System, or just **Lead**, is a **declarative build system** for expressing build outputs in terms of their dependencies. Instead of scripting commands in sequence, you describe the desired result and how it is composed.

Lead reads your build description and generates a [Ninja](https://ninja-build.org/) build file, leaving dependency tracking and parallel execution to Ninja.

## Why Lead?

- **Declarative**: Describe *what* to build, not *how* to build it.
- **Modular**: Build logic can be packaged and reused across projects without copying.
- **Pure**: Each build depends on its input parameters only — no global state — enabling multiple variants in a single configuration.
- **Extensible**: Easily combine multiple languages, targets, and configurations without rewriting the build from scratch.

## Key Concepts

### Purity

Lead is *pure* in the functional programming sense, with no side effects. This means:

- Each build is a **pure function** of its input parameters
- The same inputs always produce the same outputs
- There is no hidden global state, no environment variables that affect behavior, and no implicit dependencies
- What you see in your configuration is exactly what you get

### The parts of Lead

Lead consists of two parts, which are used together:

**lead-build** (the tool and the language) provides the `pb` command, which evaluates build descriptions written in the Lead language, a small declarative language for describing build outputs. It also provides a formatter and a language server. Learn the syntax and concepts in the [Lead Language](language/index.md) guide, and about the commands under [Tooling](tooling/index.md).

**Lead Lib** (the library) is a collection of conventions and helpers, written in the Lead language, that turn the bare language into a practical build system. It provides:

- Standardized build patterns and module interfaces
- Language helpers for C, Rust, and more
- Tools for combining multiple modules and targets
- Support for code generation, cross-compilation, and hierarchical linking

## Example

So what does it look like?

In the simplest case, it is a little more to write than in other build environments. That is the price of every part being extensible, configurable and modular, and it pays off as soon as the project grows.

```pbb
--8<-- "examples/simple/main.pbb"
```

If you want more, check out the examples in the [Cookbook](cookbook/index.md).

## Getting Started

New to Lead? Start here:

1. **[Getting Started](getting-started.md)** — Installation and a quick walkthrough
2. **[Cookbook](cookbook/index.md)** — Common build patterns with examples
3. **[Lead Language](language/index.md)** — Learn the language from the ground up

## Going Deeper

Once you're comfortable with the basics:

- **[Language Reference](language-reference/language.md)** — Complete syntax, and the [builtin functions](language-reference/builtins.md)
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

This is why declarative builds matter: each build is *pure* - it depends on its input parameters and *only* its input parameters, even if it looks a bit more complicated at first glance.

## Author

Lead is created and maintained by pengi / Max Sikström ([lead@pengi.se](mailto:lead@pengi.se)).
