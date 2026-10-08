# Pill 6 - lead-lib and multiple languages

At this point, we have built up all the building blocks needed to step into
`lead-lib` and see how to extend it from there.

Here, we are going to see how a single build can be used to build for multiple
architectures and use code generation by using code generators to output `.c`
source files.

## Migrate to lead-lib

Given the previous build, we are going to skip ahead and use `lead-lib`.

The build from pill 5 will then give the `main.pbb`, assuming lead-lib is loaded
as a subdirectory `lead-lib`, preferably as a git submodule.

```pbb
--8<-- "examples/under-the-hood/6-lead-lib/part_1/main.pbb"
```

As seen, all the building blocks are present from the previous pill.

`lib.lang.c.mod` is just a wrapper to encapsulate the boilerplate for the module
functions and enable future extension of type checking.

`lib.lang.c.app_build` encapsulates setting the `l.common.out` parameter, as it
is called in lead-lib, together with setting `l.config.target_name` to define the
name of the application.

## Adding a language

Given the flexibility of lead-lib, it is possible to add custom languages.

Languages are part of the library itself and therefore cannot be added as
modules. But by convention, `include "${cwd}/lead-lib/lead-lib.pbb" { }` takes
an object as an argument to tune lead-lib, and one way is to add the parameter
`languages`.

In this case, we are going to look at a simple "language" called "structpack",
which is a hypothetical Python package that takes a `.toml` file and converts it
into a C header file that is accessible from any other `.c` file, enabling them
to access functions generated to pack and unpack structs for communication.

The same approach would also be possible for parser generators such as `yacc` and
`bison`.

For that, we need to create a language definition file, here called
`lang_structpack.pbb`:

```pbb
--8<-- "examples/under-the-hood/6-lead-lib/part_2/lang_structpack.pbb"
```

Feel free to dig into the parts that might interest you. But in this pill, we are
going to focus on just a few parts.

First, in lead-lib, the `base` module for a language can contain parameters for
any other language too. The `base` module in this case shows how the language
itself injects an include path using `base.c.inc`. It also injects the generated
file into `base.c.extra_deps`, which the C implementation in lead-lib uses as a
dependency for all generated object files to guarantee that changes in the
`.toml` input file rebuild all other sources.

It also sets `export.mod`, which will be available as `lib.lang.sp.mod` to
simplify generation of structpack modules.

The include path in structpack is necessary because structpack generates the
header files within a build directory, so it tells structpack which translated
paths to inject.

The `main.pbb` file, with code generation, will then be:

```pbb
--8<-- "examples/under-the-hood/6-lead-lib/part_2/main.pbb"
```

The only two additions are the added `languages` to the lead-lib include and the
structpack module `my_sp_mod`.

The generated `build.ninja` can then correctly show all generated dependencies:

```ninja
rule gcc_c_o_MMD
  command = gcc -c -o ${out} -MMD -MF ${out}.d ${in} -Ibuild/my_app/model -Imodel
  description = CC$ gcc$ ${in}
  depfile = ${out}.d

rule gcc_o
  command = gcc -o ${out} ${in}
  description = LD$ gcc$ ${out}

rule python_i_o
  command = python3 structpack.py -i ${in} -o ${out}
  description = SP$ structpack$ ${in}

build build/my_app/main.o: gcc_c_o_MMD main.c | build/my_app/model/some_struct.h

build build/my_app/model/model.o: gcc_c_o_MMD model/model.c | build/my_app/model/some_struct.h

build build/my_app/model/some_other_model.o: gcc_c_o_MMD model/some_other_model.c | build/my_app/model/some_struct.h

build out/my_app: gcc_o build/my_app/main.o build/my_app/model/model.o build/my_app/model/some_other_model.o

build build/my_app/model/some_struct.h: python_i_o model/some_struct.toml

default out/my_app
```

## Conclusion

In this pill, we have shown how the previous pills integrate into lead-lib and
how to use the flexibility for code generation.

In the next pill, we will show how it can be used for multiple parallel targets
from the same modules.
