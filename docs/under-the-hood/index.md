# Introduction

In this series of pills, we introduce the concepts used in Lead Lib (`lead-lib`)
to make a complete modular build environment that can be extended with new
languages without requiring updates to lead-lib.

Note that all files demonstrated in these pills that are placed in `lib/` are
used to build up the internals of `lead-lib` itself and are not intended to be
created by users of the build system.

These pills are intended to go deep into the internals of `lead-lib` to help
understand the underlying structure of the build system, so you can adapt and
use the advanced features of the library.

We will go through how to modularize for:
- multiple targets, configuration, and cross compilation
- reusable parts of the build
- multiple languages and code generation

All in the same build configuration, by using the constructs available in
[the language description](../language/index.md).

To achieve that goal, one concept at a time needs to be introduced, which is
why the concept of "pills" is used. The first pill starts small and
incrementally builds up to what lead-lib is, based on the Lead language.

## Prerequisites

To read through the pills, it is not expected that you know every part of the
[Lead language](../language/index.md). However, it is expected that you know
the basic syntax.

Recommended to read before:

- [How to run `pb`](../language/index.md#evaluation-and-testing)
- [Core language concepts](../language/1-basics.md),
  [expressions](../language/2-expressions.md), and
  [functions](../language/3-functions.md)
- [Paths](../language/5-paths.md) - the basics: paths are a special type in
  Lead
- [Rules and builds](../language/6-build-rules.md) - in the end, we want to
  generate a ninja output file

Good to know, and read up later when needed:

- [Pattern matching](../language/3-functions.md#pattern-matching) - primarily
  how to use objects as function arguments
- [List operations](../language/4-list-operations.md) - these will be used to
  implement the pills later, but not so much for simple projects
- Operators and builtins that manipulate paths

## Examples

The examples described in the pills in this section can be found as code in the
[lead-doc](https://github.com/lead-build/lead-doc) repository, under
`examples/under-the-hood`, if you want to try the code yourself.

## The pills

* [Pill 1 - **Basic builds**](1-builds.md) - separation of *what* to build from
  *how* to build it.
* [Pill 2 - **Modules**](2-modules.md) - introduction of the concept of having
  source code modules and including libraries instead of source files.
* [Pill 3 - **Configuration**](3-configuration.md) - how to use modules
  to manage compiler configuration.
* [Pill 4 - **Dependent variables**](4-dependent-vars.md) - looking at how module
  parameters can depend on each other.
* [Pill 5 - **Generic build function**](5-generic-build.md) - how to achieve a
  language-independent build function.
* [Pill 6 - **lead-lib and multiple languages**](6-lead-lib.md) - how this fits
  in with `lead-lib`, and how to add a language for code generation.
* [Pill 7 - **Multiple targets**](7-multiple-targets.md) - how to build several
  configurations in parallel from the same modules.
