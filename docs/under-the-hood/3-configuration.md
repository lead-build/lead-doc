# Pill 3 - Configuration

Up to this point, we have added modular builds and the possibility to merge
modules. But we do not yet have any support for adding configuration, such as
compiler flags or even which compiler to use.

This part is not going to be too advanced, but it is important for understanding
the concept of the lead-lib module system. We will extend it again later.

## Modules

Modules have, until now, only been source files for the C language, with no
configuration.

However, to be able to add more languages and generic configuration, it is
important to be able to add more categories to the modules, one per "language".

In lead-lib, a module category is called a "language", since the intended
purpose is to group parameters into their respective language. However, there
are also common parameters and configuration parameters, which are placed in
the respective language for generality—for example, `config`.

## Implementation

To start with, we look at how a `main.pbb` can look when we add some
configuration to a module.

At this point, we keep the `objdir` and `app` as lists for now, for simplicity,
and expect them to be a single value at the end. When introducing `lead-lib`
later, this will be more generic.

```pbb
--8<-- "examples/under-the-hood/3-configuration/main.pbb"
```

In this file, we can show that a module can also contain configuration and
categories per language.

We have added three configuration parameters:

- The application name, which is what we want to generate
- The `srcdir`, which is the base directory to find all source files
- The `objdir`, which is where to place the object files

The app name and `objdir` should be a single variable, but for simplicity in this
pill, we treat them as lists and use the last value in the list when building.
This lets us focus on the concept of parameters and modules rather than how to
handle different types. We will address that later, when introducing `lead-lib`
in a later pill.

The goal is that each source file should be compiled into an object file with
the relative path to `objdir`, matching the source file's relative path to
`srcdir`. For example, `module/lib.c` should result in `build/module/lib.o`, and
then be linked into `my_app`.

The eagle-eyed reader may have noticed that the language library included
previously is not a generic library; it is the C language library used by the
build via `lib.c.build`.

But first, define what's necessary for the `config` language by implementing
`lib/lang_config.pbb`:

```pbb
--8<-- "examples/under-the-hood/3-configuration/lib/lang_config.pbb"
```

Note that the config language so far does not have a build function, since we
only intend to build the C language.

Speaking of `lib/lang_c.pbb`, that also needs to be updated:

```pbb
--8<-- "examples/under-the-hood/3-configuration/lib/lang_c.pbb"
```

In this file, we have made some changes.

Next, the build takes not only the parameters from the `c` language, but also
all config. This means `lib.c.build` has access to `mod.config.xxx`
parameters. Therefore, `lib.c.build` no longer needs the extra config parameter;
configuration is a module itself.

The `lead-build` builtin `pb.translate` is therefore used to remap the source
files to the build directory based on what is available in the config.

We still need to implement `lib/lib.pbb` though:

```pbb
--8<-- "examples/under-the-hood/3-configuration/lib/lib.pbb"
```

In this case, we re-export the `lang_c` and `lang_config.pbb` as `lib.c` and
`lib.config`, while implementing a merge function that merges all language parts
individually. `mod ? "language"` is a `lead-build` language construct used to
check whether a field is defined in an object.

Refer to [lead-build documentation](https://lead-build.readthedocs.io) if you
want to read more about how list comprehensions work. Otherwise, it is simply
filtering out each language submodule from a list of modules.

# Conclusion

In this pill, we have looked at how to place configuration in a module to keep
a build entirely as a set of modules.

It is a big step forward toward being generic, but we still have one part
configured outside of the list of modules. We still use `lib.c.build`, not a
generic `lib.build`, for building. So we still assume it is a C project, not a
generic project containing C.

In the next pill, we will step back from the actual builds and look at a concept
for making parameters in a module dependent on other parameters, before going
back to see how we can generalize the final part of the module concept.