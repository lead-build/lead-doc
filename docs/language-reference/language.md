# Language reference

This document provides a reference guide for the Lead language syntax,
operators, and language constructs. Use this guide to understand the core
language features and their usage.

## Types

Multiple types are available in the langauge. Those available to the user are
listed below.

### Numbers

Numeric literals represent integer.

Example: `42` or `-23`

### Boolean

Boolean values are `true` or `false`. They are used for conditionals and logical
expressions.

Example: `true` or `false`

### Null

The literal `null` represents the absence of a value. `null` is only equal to
itself: `null == null` is `true`, and comparing `null` to any other value with
`==`/`!=` is `false`/`true` respectively.

Example: `null`

### Strings

Strings are quoted text values. They may contain escaped characters such as `\"`
for a quote and `\\` for a backslash. Strings are commonly used for path
segments and textual data.

Strings may also contain sequence `${expr}` containing a sub expression. Usually
for embedding a variable. This is identical to concatenation of the parts using
`+` operator.

Example: `"hellorld"` or `"Hello ${name}!"`

### Objects

Objects are collections of named fields written with `{ key = value; ... }`.
They are used for structured data, function arguments, and configuration.

Object keys may be written either as identifiers (`name`) or as quoted strings
(`"compiler-flags"`).

A field written as a bare identifier, without `= value;`, is shorthand for
`identifier = identifier;`: the value is taken from a variable of that name in
the enclosing scope. This shorthand is only available for identifier keys, not
quoted string keys.

Objects support `==`/`!=`: two objects are equal when they have the same set of
keys and each corresponding value is equal.

Example:
```pbb
{
  src = "src";
  out = "build";
}
```

### Lists

Lists are ordered collections written as `[item1, item2, ...]`. They hold a
sequence of values of any type.

Items are separated with `,`. Last element may have an optional trailing `,`.

Example:
```pbb
[
    "a.c",
    "b.c",
    "c.c"
]
```

### Paths

Paths represent filesystem locations and may refer to either files or
directories. Paths are built from builtin values such as `cwd` and are
manipulated with the `/` operator.

Example:
```pbb
|{cwd, ...}|
let
  src = cwd / "src" / "main.c";
in
  src
```

#### Path locking

Paths are locked to a given directory, and can never traverse outside of the
directory.

Failing example: `srcdir / ".."` will not work due to locking.
```pbb
|{cwd, ...}|
let
  srcdir = pb.lock (cwd / "src");
  back = srcdir / "..";
in
  back
```


### Builds

Build objects are returned by calling the function produced by `pb.rule`, represneting a build operation.

### Build rules

Build rule functions are returned by internal `pb.rule` represneting a build rule.

#### Build rule placeholder object

A placeholder for a variable, available internally in the function defining a `pb.rule`.

## Expressions structure

Following operators are available:

| Operator                     | Precedence | Description                       |
| ---------------------------- | ---------- | --------------------------------- |
| `let ... in ...`             | 1          | let block expression              |
| `|matcher| expr`             | 1          | Function definition               |
| `lhs -> rhs`                 | 2          | Logical implication               |
| `lhs || rhs`                 | 3          | Logical or                        |
| `lhs && rhs`                 | 4          | Logical and                       |
| `lhs == rhs`                 | 5          | Equal                             |
| `lhs != rhs`                 | 5          | Not equal                         |
| `lhs < rhs`                  | 6          | Less than                         |
| `lhs <= rhs`                 | 6          | Less than or equal                |
| `lhs > rhs`                  | 6          | Greater than                      |
| `lhs >= rhs`                 | 6          | Greater than or equal             |
| `lhs // rhs`                 | 7          | Object update/merge               |
| `!expr`                      | 8          | Logical not                       |
| `lhs + rhs`                  | 9          | Addition or string concatentation |
| `lhs - rhs`                  | 9          | Subtraction                       |
| `lhs * rhs`                  | 10         | Multiplication                    |
| `lhs / rhs`                  | 10         | Division or path extension        |
| `lhs ++ rhs`                 | 11         | List concactenation               |
| `lhs ? rhs`                  | 12         | Has attribute                     |
| `-expr`                      | 13         | Numeric negation                  |
| `func arg`                   | 14         | Function call                     |
| `object.ident`               | 15         | attribute selection               |
| `object.{expr}`              | 15         | computed attribute selection      |
| `( func for init: list )`   | 16         | list fold                         |
| `[ func for iterable ]`      | 16         | list map                          |
| `{ func for iterable }`      | 16         | object map                        |
| `switch expr { ... }`        | 16         | switch expression                 |
| `match expr { ... }`         | 16         | match expression                  |


### Function defintion

A function is represented as `|matchers| expr`, where matcher is a list of zero or more matchers,
separated by space, providing input variables to the expression `expr`.

Matchers can be any matcher defined in the [matcher chapter](#matchers) below.

No matchers in the list is equivalent to no function definition at all. Thus `|| expr` is equal to `expr`

Multiple matchers is equal to multiple nested functions. Thus `|a b| expr` is equal to `|a| |b| expr`.

More information available in [function chapter](../language/3-functions.md) in the documentation.

### Logical "and" and "or"

`&&` and `||` is evaluated lazy.

- Given `rhs && lhs`, and `rhs` is false, then `lhs` is not evaluated.
- Given `rhs || lhs`, and `rhs` is true, then `lhs` is not evaluated.

### List and object iteration

Lists and objects can be iterated over, using the *map* construct.

The map construct has two variants:

- generate a list: `[ func for iterable ]`
- generate an object: `{ func for iterable }`

Both forms also support an optional filter clause:

- generate a filtered list: `[ func for iterable if filter ]`
- generate a filtered object: `{ func for iterable if filter }`

`iterable` can be either a list or object, and `func` is a function that
transform each element in iterable to an output element.

When present, `filter` is evaluated before `func` for each input element.
Only elements where `filter` evaluates to `true` are kept. `filter` is a
function taking the same argument as `func`, returning `true` or `false`

For lists, element is passed, or received directly. For objects, each element
is represented to `func` as a tuple of `(key, value)`, both as argument and
return value.

The filter receives the same input representation as the mapper, meaning list
elements for list input and `(key, value)` tuples for object input.

This means four combinations, plus the four with filter:

| From   | To     | Representation                                                              |
| ------ | ------ | --------------------------------------------------------------------------- |
| List   | List   | `[ |input_val| output_val for input_list ]`                                 |
| List   | Object | `{ |input_val| (output_key, output_value) for source_list }`                |
| Object | List   | `[ |(input_key, input_val)| output_val for input_object ]`                  |
| Object | Object | `{ |(input_key, input_val)| (output_key, output_value) for source_object }` |

Example with filtering:

```pbb
[ |a| a * 2 for [1, 2, 3] if |a| a < 3 ]
```

### Switch expressions

Switch expressions choose the first matching case from a series of value comparisons.

```pbb
switch arch {
  "x86_64" => "desktop";
  "riscv64" => "embedded";
  _ => "unknown";
}
```

The general shape is:

```pbb
switch expr {
  matcher => result;
  _ => default_result;
}
```

Cases are checked in order. The first matcher equal to the switch value wins, and the optional `_` case acts as a default when no case matches.

### Match expressions

Match expressions choose the first case whose matcher fits the shape of the value.

```pbb
match args {
  [] => "none";
  [single] => single;
  [first, second, ...] => "${first} ${second}";
  _ => "not a list";
}
```

The general shape is:

```pbb
match expr {
  matcher => result;
  matcher => result;
}
```

Cases are checked in order, and the first [matcher](#matchers) that fits the value wins. Variables bound by that matcher are available in the result of that case only, and shadow variables of the same name from the surrounding scope.

There is no separate default case; since the matcher `_` fits any value, a last case of `_ => result;` acts as a default. If no case fits, evaluation fails with an error.

Unlike `switch`, no values are compared. A matcher only describes a shape, so `match` can tell a list from a tuple, lists of different lengths, or objects with different fields, but not `1` from `2`.

## Matchers

Matchers unpack a value into variables. They are used for function arguments, in `let ... in ...` bindings, and for the cases in a `match` expression.

| Matcher                   | Fits                                                    | Binds                             |
| ------------------------- | ------------------------------------------------------- | --------------------------------- |
| `name`                    | Any value                                               | The value as `name`               |
| `_`                       | Any value                                               | Nothing                           |
| `(m1, m2)`                | A tuple of exactly that size                            | What each sub-matcher binds       |
| `[m1, m2]`                | A list of exactly that length                           | What each sub-matcher binds       |
| `[m1, m2, ...]`           | A list of at least that length                          | What each sub-matcher binds       |
| `{ f1, f2 }`              | An object with exactly those fields                     | What each field matcher binds     |
| `{ f1, f2, ... }`         | An object with at least those fields                    | What each field matcher binds     |
| `matcher @ name`          | Whatever `matcher` fits                                 | Same as `matcher`, and the entire value as `name` |

As for tuple expressions, a tuple matcher of a single element is written with a trailing comma: `(m1,)`.

In a function argument or a `let` binding, a value that doesn't fit the matcher is an error. In a `match` expression, the next case is tried instead.

### List matchers

A list matcher `[m1, m2, m3]` fits a list of exactly as many elements as there are sub-matchers, matching each element with the sub-matcher in the same position. `[]` fits only the empty list.

A list matcher ending with `...` matches a prefix of the list. `[m1, m2, ...]` fits a list of two or more elements, matching the first two, and ignoring the rest. The `...` stands for zero or more elements, so `[...]` fits any list.

`...` is only allowed as the last element, and the elements it stands for are not bound to a variable. Use `[m1, ...] @ name` to also get the entire list.

```pbb
let
  [first, second, ...] = [10, 20, 30, 40];
in
  first + second
```

This evaluates to `30`.

### Object matchers

An object matcher is a list of field matchers separated by commas, each one of:

| Field matcher               | Description                                                            |
| --------------------------- | ---------------------------------------------------------------------- |
| `name`                      | Binds the field `name` to a variable of the same name                  |
| `name = matcher`            | Matches the field `name` using `matcher`                               |
| `name ? default`            | As `name`, using the expression `default` if the field is missing      |
| `name = matcher ? default`  | As `name = matcher`, using the expression `default` if the field is missing |

A field that is missing, and has no default, doesn't fit. Fields in the object that are not named in the matcher don't fit either, unless the matcher ends with `...`.

More information and examples are available in the [function chapter](../language/3-functions.md#pattern-matching) in the documentation.
