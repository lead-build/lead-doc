# Functions and Pattern Matching

Functions make it possible to reuse behavior and patterns.

## Functions

Functions are themselves a compound type, which can be invoked to transform an input argument into an output.

They use the syntax: `|argument| output`

Here, `argument` is a variable pattern, of the same form as in `let ... in ...` statements described in the previous chapter.

They are normally assigned to a variable or an element in an object, to be invoked later. But since they are values themselves, they can also be passed as arguments or stored in lists.

Invocations are done by referencing the function followed by the argument. For example: `<func> <arg>`.

For example:

```pbb
let
  add_three = |x| x + 3;
in
  {
    a = add_three 1;
    b = add_three 2;
    c = add_three 3;
  }
```

This evaluates to:

```pbb
{
  a = 4;
  b = 5;
  c = 6;
}
```

Functions do not have to be stored in variables. It is valid to call them directly. For example, `(|a| a + 10) 3` evaluates to `13`.

## Scopes and bound variables

When declaring functions, values from the scope where they are declared may be referenced from the function.

```pbb
let
  add_val = 3;
  add_three = |x| x + add_val;
in
  {
    a = add_three 1;
    b = add_three 2;
    c = add_three 3;
  }
```

This is still valid. When `add_three` is created, it has `add_val = 3` in its scope, which is bound to the function.

Since the scope follows the function and cannot be updated later, it is still pure: the behavior of the function will be the same wherever it is called and will depend only on its input.

## Multiple arguments

A function by design only takes one argument and returns a single value. But often it is necessary to take multiple arguments or return multiple values.

Returning multiple values is achieved by using one of the compound types, typically an object, but lists and tuples are also possible.

To make a function take multiple arguments, it is also possible to take a compound type, which can be helped by variable patterns and pattern matching, as described later.

It is also possible to leverage the concept of bound scopes, as described above, by making a function return another function.

For example:

```pbb
let
  add = |a| |b| (a + b);
in
  (add 3 5)
```

In the example above, `add 3 5` is resolved as `(add 3) 5`. The expression `add 3` is evaluated using the outer function, returning `|b| (a + b)`, where `a` is bound to `3` from the input parameter and can therefore be written as `|b| (3 + b)`.

When replacing `add 3` with the value above, `(|b| (3 + b)) 5` evaluates to `3 + 5`, or `8`.

As syntactic sugar, it is possible to skip the middle `|`, writing multiple arguments as `|a b| (a + b)`.

This makes it possible to create new functions where earlier arguments are already set. For example:

```pbb
let
  add = |a b| a + b;
  add_three = add 3;
in
  (add_three 5)
```

## Pattern matching

The other method of sending multiple arguments to the same function is by using compound types. To make it more convenient to select and unpack the compound type into individual variables, it is possible to pattern match on the types.

The same mechanism is used in the `let ... in ...` construct when assigning variables, as described in earlier chapters.

A pattern can be one of:

- variable - a single name, representing the entire argument
- tuple pattern - written as a tuple `(a, b, c)`, where each field is itself a pattern, expecting a tuple of the correct size
- list pattern - written as a list `[a, b, c]`, where each element is itself a pattern, expecting a list of the correct length. Ending it with `...`, as in `[a, b, ...]`, allows the list to be longer
- object pattern - written as a pattern surrounded by braces `{...}`, containing a set of match statements separated by commas
- wildcard pattern - a single `_`, matching anything, but ignoring the value and not storing it

An object pattern contains a set of match statements, which can either be:

- a single name, matching the name in the object to a variable of the same name
- a statement `name = submatcher`, matching the field `name` in the input object using the `submatcher`. This means `{ field = varname } = obj;` unpacks `obj`, placing the field `field` into the variable `varname`

Optional default values can be added with `name ? default_expr` or `name = submatcher ? default_expr`. If the field is missing, the default expression is used.

If a field is missing and no default is defined for that field, matching fails with an error.

An object pattern normally expects all fields to be extracted; otherwise it will return an error. If only a few fields are expected to be unpacked but more fields should be allowed, the object matcher can end with a `...` statement, allowing extra fields.

A list pattern normally expects the list to have exactly as many elements as the pattern; otherwise it will return an error. If only the first elements are of interest, the list pattern can end with `...`, which accepts any number of further elements, including none. So `[a, b, ...]` matches any list with at least two elements, and `[...]` matches any list. The `...` is only allowed at the end, and the remaining elements are not stored anywhere; use `@` as described below if the entire list is needed.

Any matcher, except single-name object matchers, can be combined with `matcher @ variable`, causing the entire field to be stored in the variable while also being matched by the matcher. This is useful when extracting some fields of an object while still having access to the object. For example: `|{var, ...} @ obj| ...`.

For example:

```pbb
let
  myfunc = |{name, ...} @ obj| { name = name; type = "obj"; inner = obj; };
in
  myfunc {
    name = "test";
    local_var = "something";
  }
```

This evaluates to:

```pbb
{
  inner = {
    local_var = "something";
    name = "test";
  };
  name = "test";
  type = "obj";
}
```

This shows that the original argument is copied in its entirety to `inner` in the result, while `name` is unpacked.

This can also be applied to tuples:

```pbb
let
  add = |(a, b)| a + b;
in
  add (3, 4)
```

This evaluates to `7`.

And to lists, here picking out the first two elements of a longer list:

```pbb
let
  [first, second, ...] @ all = [10, 20, 30, 40];
in
  {
    sum = first + second;
    all = all;
  }
```

This evaluates to:

```pbb
{
  all = [10, 20, 30, 40];
  sum = 30;
}
```

Default values are especially useful for optional object fields in matcher patterns:

```pbb
let
  add_with_default = |{a, b ? 12}| a + b;
in
  add_with_default { a = 3; }
```

This evaluates to `15`.

## Match expressions

In a function argument or a `let` binding, a pattern that doesn't fit the value is an error. A `match` expression instead takes a list of patterns, and picks the first one that fits:

```pbb
let
  describe = |value| match value {
    [] => "empty list";
    [only] => "list with one element";
    [first, second, ...] => "list with at least two elements";
    (a, b) => "pair";
    { name, ... } => "object named ${name}";
    _ => "something else";
  };
in
  [
    describe [],
    describe [1, 2, 3],
    describe (1, 2),
    describe { name = "test"; size = 3; },
    describe 17,
  ]
```

This evaluates to:

```pbb
[
  "empty list",
  "list with at least two elements",
  "pair",
  "object named test",
  "something else",
]
```

Each case is written as `pattern => expression;`, where the pattern is any pattern described above. The cases are tried in order, and the first pattern that fits the value is used. The variables from that pattern are available in the expression of that case, and only there.

A pattern fits the value if the value has the shape the pattern describes:

- a variable or `_` fits any value
- a tuple pattern fits a tuple of the same size
- a list pattern fits a list of the same length, or with `...`, a list of at least that length
- an object pattern fits an object having the fields of the pattern, and no other fields unless the pattern ends with `...`

If no case fits, it is an error. A last case of `_ => ...` therefore works as a default.

Note the difference from `switch`: a `switch` compares the value for equality with each case, while a `match` only looks at the shape of the value. A pattern can not contain a value to compare with, so a case of `x => ...` doesn't compare the value with a variable `x`; it fits anything, and stores it in a new variable `x`. To choose on both shape and value, use a `switch` on the variables inside the `match` case.

## File header and builtins

At this point, all the pieces are there to revisit the file header.

Looking at the first example:

```pbb
|{...}|
let
  a = 13;
in
{
  variable = a;
}
```

it is quite clear that the file actually contains a function taking an object as its argument and matching any extra unspecified fields.

That is because the file itself contains a single pure expression. Any builtins are passed as arguments to the function. Builtins are functions or arguments built into the `pb` tool itself and exposed to the implementation.

For example, to have access to the `include` function, it is possible to pattern match on the field `include` in the header:

```pbb
|{include, ...}|
...
```

The builtins available in the header are:

- `cwd` - the path of the directory containing the current file, described in the chapter on [paths](5-paths.md)
- `include` - a function that loads another `.pbb` file, described below
- `pb` - functions for paths and builds, described in the chapters on [paths](5-paths.md) and [build rules](6-build-rules.md)
- `ops` - operations on lists and objects
- `dbg` - helpers for debugging

They are all listed in the reference of [builtin functions](../language-reference/builtins.md).

## Multiple files

The builtin `include` takes the path of another `.pbb` file, and returns what that file evaluates to. The file header of the included file gets its own builtins, where `cwd` is the directory of the included file itself.

For example, given a file `values.pbb`:

```pbb
|{...}|
{
  name = "demo";
  version = 3;
}
```

it can be used from another file in the same directory:

```pbb
|{cwd, include, ...}|
let
  values = include (cwd / "values.pbb");
in
  values.version + 1
```

This evaluates to `4`.

If the included file should take arguments, the common pattern is to let it evaluate to a function, by adding a second set of arguments after the file header. Given `greet.pbb`:

```pbb
|{...}|
|name|
"hello ${name}"
```

then `include (cwd / "greet.pbb") "world"` evaluates to `"hello world"`.
