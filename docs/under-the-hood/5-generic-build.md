# Pill 5 - Generic build function

So far, apart from pill 4, we have used a specific function to handle C builds.

But one of the core concepts of the Lead Build System, with `lead-lib` and
`lead-build`, is that you never create a language-specific project, like a C
project. Instead, you create a project containing C code or a combination of
languages.

Another core concept is that there are no global configuration values; instead,
configuration resides within the modules passed to `lib.build`.

Up until this point, we have built up the concepts required to achieve this; it
is now just a matter of fitting the pieces together.

## Generalizing the build

In pill 3, we showed how to add configuration as modules. In pill 4, we showed
how to make parameters in the modules dependent on each other while allowing
non-referenced parameters to remain unevaluated due to laziness in
`lead-build`.

This solution can be used to define what outputs the project should have by
defining the common `config.out` as a list of all generated artifacts.

Given that languages inject a base module before the others in the last step
before building, it is possible for the language to always add a build in its
own submodule.

For example, `lang.c` can add `lang.c.app` in its module, and that will always
point to a generated build based on the parameters specified in `lang.c.srcs`,
`lang.c.incs`, and others. If it is not referenced—for example, when building a
Rust output—it does not even need to compile, since it is not evaluated.

This means that to tell `lib.build` to output the application build from C, it is
sufficient to add a module containing:

```pbb
app_build = | self | {
    config = {
        out = self.c.app;
    };
};
```

Adding this module to the build will result in `lib.build` referencing
`self.config.out`, which then references `self.c.app`.

## Adapting the build to dependent variables

To show how it works, let's adapt the build from pill 3 with the dependent
variables from pill 4 and add the output as described above.

To start, we look at what implications this has for `main.pbb`, which is what
the user sees:

```pbb
--8<-- "examples/under-the-hood/5-generic-build/main.pbb"
```

The main parts that are added are simply the encapsulation of the objects as
functions. Most of them are wrapped in helpers anyway.

Let's look at `lib/lib.pbb`:

```pbb
--8<-- "examples/under-the-hood/5-generic-build/lib/lib.pbb"
```

Here are some larger changes.

First, the merge function takes an extra argument. Due to how `lead-build`
handles functions with multiple arguments, calling the function with only `mods`
*binds* `mods` to the function and leaves a function taking `self`.

The only other change is that it performs the recursive evaluation of all mods
to continue the merge as before.

The second change is `base_mod`, which is built up from all the languages
available and is needed by the `build` function, as described in pill 4. It can
be seen that it combines `base_mod` with `mod` to recurse before getting
`res.config.out`.

The `lib/lang_config.pbb` does not contain much that is surprising:

```pbb
--8<-- "examples/under-the-hood/5-generic-build/lib/lang_config.pbb"
```

The base submodule contains the defaults, and `merge` has the new `out` field.

For `lib/lang_c.pbb`, it is mostly the same, only the `base`, with a new `app`
field that exposes the build. `app` is also added to the merge:

```pbb
--8<-- "examples/under-the-hood/5-generic-build/lib/lang_c.pbb"
```

The build function is actually untouched. What has happened is that it is moved
so it is no longer exported, but rather used in the new `base` module as the new
parameter `app`, as described earlier.

## Conclusion

This shows that it is possible to use a generic project and a generic build
function, `lib.build`, to build any language.

This gives a modular system where any parameter, even the list of sources, can
be modified from any other module or language, which is useful for code
generation, as we will see in the next pill, where we introduce `lead-lib` and
more languages.