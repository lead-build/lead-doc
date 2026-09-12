# Pill 4 - Dependent Variables

In this pill, we are going to step back a bit from the bigger project and look
at the module concept in its basic form.

So far, we have shown how to make modules that model the source files and
parameters from a submodule, and how to place compiler configuration in a
module. However, there is a limitation: parameters cannot depend on one another.

To do this, we are not going to generate a ninja build file, but instead use the
`pb -E` feature to look at the evaluated lead source tree.

## Simplification

At the core, we are going to look at a simple module of one level:

Create a file `part_1.pbb`:

```pbb
--8<-- "examples/under-the-hood/4-dependent-vars/part_1.pbb"
```

This shows a generated path from the three parameters in the simplified module.

Now try to evaluate it using `pb -E -i part_1.pbb`. It should output:

```
[root]/$/out/my_app-x86_64
```

Note that the path is written as a Path object, as described in the
[lead-build](https://lead-build.readthedocs.io) documentation.

The question is how we can generate a parameter within the module itself for the
output file, dependent on the three parameters. For example:

```pbb
|{ cwd, ... }|
let
    mod = {
        app = "my_app";
        arch = "x86_64";
        outdir = "${cwd}/out";
        outfile = ...; # What to put here?
    };
in
mod.outfile
```

## Iteration

One important aspect of `lead-build` is that it is *lazy*, which means it only
evaluates the parts of the expressions that are actually used. This can be used
to our advantage.

So let's try again, but make the module a *function*.

Create a `part_2.pbb` containing:

```pbb
--8<-- "examples/under-the-hood/4-dependent-vars/part_2.pbb"
```

Since nothing within the module depends on `l`, we can simply pass an empty
object to it.

The result is the same as the previous attempt:

```
[root]/$/out/my_app-x86_64
```

Now we have the possibility to pass a value to the object itself, which means we
can create parameters in the module dependent on some input parameters. What if
we then run the module twice, passing the first iteration back to itself?

Let's try `part_3.pbb`:

```pbb
--8<-- "examples/under-the-hood/4-dependent-vars/part_3.pbb"
```

This gives yet again:

```
[root]/$/out/my_app-x86_64
```

Note that in this case, if we looked at `iter1.outfile`, that would be an error,
since `l.outdir`, `l.app`, and `l.arch` would be undefined for `iter1`, where `l`
is set to `{}`.

It is nevertheless valid to have `outfile` set dependent on those variables in
`l`, since `outfile` is not *evaluated* as long as `l.outfile` is not referenced
at that iteration.

In this approach we have shown one level of recursion, which is enough to
resolve one reference. What if we need more levels of referencing?

## Recursion

Ulitimately, this would be done infinitely, but to start with, we set "infinite"
to 5 iterations, and try to make it generic instead.

To start with, look at the concept of a couple of iterations:

```pbb
mod = |self| ...;

iter1 = null;
iter2 = mod iter1;
iter3 = mod iter2;
iter4 = mod iter3;
iter5 = mod iter4;
out = mod iter5;
```

This maps to the operation of fold, so that:
```pbb
mod = | l | ...;

out = (|prev _| mod prev for null:[1,2,3,4,5]);
```

The list `[1,2,3,4]` is there only to provide something to iterate over. The
value is ignored, as the second argument to the iterator function, `_`, and the
iteration starts at `null`. The values are semantically identical.

The construct in the second line can be rewritten as a helper function:
```pbb
recurse = |mod| (|prev _| mod prev for null:[1,2,3,4,5]);
```

Lets add it to the code above as `part_4.pbb`:

```pbb
--8<-- "examples/under-the-hood/4-dependent-vars/part_4.pbb"
```

To allow infinite recursion, the list needs to be an infinite list. However,
to also fully leverage the laziness property of the language, this recurse
function, with infinite iterations, is implemented as a builtin to the language,
called `ops.recurse`.

lets try it out, as `part_5.pbb`:
```pbb
--8<-- "examples/under-the-hood/4-dependent-vars/part_5.pbb"
```

This concept of recursive resolution is used as the basis of the `lead-lib`
module system, to make it possible to allow multiple languages to depend on each
other, as will be described in later pills.

More levels of iteration can be seen here, in `part_6.pbb`:

```pbb
--8<-- "examples/under-the-hood/4-dependent-vars/part_6.pbb"
```

This resolves to:

```
"abcd"
```

## Infinite recursion

As shown in the previous chapter, this is a powerful concept for allowing
dependencies between parameters.

However, it only works if it eventually ends up in parameters that are not
dependent on `l`. If not, there will be no termination, and the system will end
up in an infinite recursion loop that is quite hard to debug.

One example is `part_7`:

```pbb
--8<-- "examples/under-the-hood/4-dependent-vars/part_7.pbb"
```

This shows that `self.var_b` depends on `self.var_a`, which depends back on
`self.var_b`.

Running this example will result in:

```
thread 'main' (391980) has overflowed its stack
fatal runtime error: stack overflow, aborting
[1]    391980 IOT instruction  pb -E -i part_7.pbb

```

To have any chance of debugging this, add `-vvv` to `pb`. It will print out a
trace of all expressions being resolved internally within `pb`. The format may
vary between versions of `pb`, but it may provide a clue as to where the
recursion happens.

`pb -E -i part_7.pbb -vvv` will be quite noisy, but it will give some clues.

In this case, it will output something similar to:

```
...    ...    binary operator expression at part_7.pbb:4:17
... Resolving bound expression at part_7.pbb:4:17
...    ...    attribute selection at part_7.pbb:4:17
... Resolving bound expression at part_7.pbb:4:22
... Resolving bound expression at part_7.pbb:4:17
... Resolving bound expression at part_7.pbb:7:23
...    ...    function call at part_7.pbb:7:23
... Resolving bound expression at part_7.pbb:7:23
...    ...    bound expression at part_7.pbb:7:23
...    ...    bound expression at part_7.pbb:3:18
... Resolving bound expression at part_7.pbb:5:17
...    ...    binary operator expression at part_7.pbb:5:17
... Resolving bound expression at part_7.pbb:5:17
...    ...    attribute selection at part_7.pbb:5:17
... Resolving bound expression at part_7.pbb:5:22
... Resolving bound expression at part_7.pbb:5:17
... Resolving bound expression at part_7.pbb:7:23
...    ...    function call at part_7.pbb:7:23
... Resolving bound expression at part_7.pbb:7:23
...    ...    bound expression at part_7.pbb:7:23
...    ...    bound expression at part_7.pbb:3:18
... Resolving bound expression at part_7.pbb:4:17
```

In this case, it can be seen that there are a lot of focus around `part_7.pbb:4`
and `part_7.pbb:5`, which happens to be the lines:

```pbb
        var_a = self.var_b + "a";
        var_b = self.var_a + "b";
```
which gives a clue of where the infinite recursion happens.

## Merge

This however means we need to modify the merging slightly, by passing the `self`
parameter forward.

Given that we have a `merge_objs` function, that merges two resolved modules,
the `self` parameter can be passed down simply by using:

```pbb
merge = | mods self | merge_objs [|mod| mod self for mods];
```

## Conclusion

We have introduced the concept of having modules as functions with recursion,
to make module parameters dependent on each other.

In the next pill, we will look at how this self-referencing can be used to
replace the language specific `lib.c.build` with a generic `lib.build`.
