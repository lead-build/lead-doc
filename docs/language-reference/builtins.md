# Builtin functions

The builtins are passed to each `.pbb` file as fields of the argument to the
[file header](../language/3-functions.md#file-header-and-builtins).

Besides the builtin values `cwd`, the path to the directory of the current file,
and `include`, a function that loads another `.pbb` file, the builtins are split
into three categories:

- `ops` - operations, to access basic structures, like native objects and lists.
- `pb` - access build related values, like paths and builds.
- `dbg` - for debugging

## Operations

### `ops.transpose`

Takes a compound of compounds, and transposes the keys, such that:

for any input element `a[x][y]`, ends up in the output element `b[y][x]`.

A compound is either a list, tuple or object.

For example:
```pbb
a = {
  var_a = [1,2,3];
  var_b = [4,5,6];
};

b = ops.transpose a;
```
produces:
```pbb
b = [
  { var_a = 1; var_b = 4; },
  { var_a = 2; var_b = 5; },
  { var_a = 3; var_b = 6; },
];
```

Any missing index is padded with `null`.

### `ops.transposeObjs`

Special case of `ops.transpose`, taking a list of objects, returning an object
of lists.

The difference from `ops.transpose` is that the inner lists are not padded with
`null`, and thus do not maintain the indexes.

```pbb
a = [
  { x = 1; y = 2; },
  { x = 3; z = 4; },
];

b = ops.transposeObjs a;
```
produces:
```pbb
b = {
  x = [1, 3];
  y = [2];
  z = [4];
};
```

### `ops.recurse`

Calls a function with its own output as argument.

It is roughly equivalent to:

```pbb
recurse = |f| (|prev _| f prev for null : [1,2,3,4,5...infinite]);
```

so that the following two lines are functionally identical:

```pbb
out = ops.recurse func
out = func (func (func (func ...infinitely...)))
```

It uses the fact that in the above, `out == func out`. Together with lazy
evaluation, that means a field of `out` (given that `out` is an object) doesn't
need to be reevaluated for each level of recursion.

More information is available in the pill on
[dependent variables](../under-the-hood/4-dependent-vars.md).

## Builtin path functions

### `pb.translate`

Rewrites a path by replacing a directory prefix.

Syntax:
```pbb
pb.translate {
  input = path;
  from = path or [path, ...];
  to = path;
}
```

- `input`: the path to rewrite
- `from`: one path or a list of candidate base path prefixes that may contain `input`
- `to`: the directory to use instead of `from`

Returns a path where the matching `from` prefix is removed from `input` and replaced by `to`.
If `from` is a list, the first matching prefix is used.

### `pb.rebase`

Converts a path into a string, expressing the path relative to another base path.

Syntax:
```pbb
pb.rebase {
  path = path;
  base = path;
}
```

- `path`: the path value to convert
- `base`: the directory the result should be relative to

Returns a string with the relative path from `base` to `path`. For example, rebasing `cwd / "src" / "main.c"` to the base `cwd / "build"` gives `"../src/main.c"`.

### `pb.lock`

Creates a new path value bound to the same file or directory, but with a fresh root boundary, so upward traversal (`..`) cannot escape above it.

Syntax:
```pbb
pb.lock path
```

More information is available in the [paths](../language/5-paths.md) chapter.

### `pb.matches`

Matches a path or string against a glob pattern. The function is curried: it takes the pattern first, and returns a function that takes the path or string to test and returns a boolean.

Syntax:
```pbb
pb.matches pattern subject
```

- `pattern`: a string containing a glob pattern
- `subject`: a path or a string to match against the pattern

Returns `true` if `subject` matches `pattern`, otherwise `false`. An invalid pattern, or a `pattern` that isn't a string, is an error, as is a `subject` that is neither a path nor a string.

The pattern syntax is:

- `?` matches any single character except `/`
- `*` matches any number of characters, except `/`
- `**` matches across directories, for example `**/src/**` or `src/**/*.c`
- `[ch]` matches one of the listed characters, and `[!ch]` any character not listed
- `{a,b}` matches either of the alternatives

The whole subject must match. A pattern without any `/` is matched against the last component of the subject only, so `*.c` matches both `main.c` and `src/main.c`. A pattern containing a `/` is matched against the whole subject, so `src/*.c` matches `src/main.c` but not `src/sub/main.c`.

When the subject is a path, the part of the path below its lock boundary is matched, using `/` as separator. See [`pb.lock`](#pblock).

```pbb
|{cwd, pb, ...}|
let
  is_c = pb.matches "*.{c,cpp}";
in
  [
    is_c (cwd / "src" / "main.c"),              # true
    is_c "main.h",                              # false
    pb.matches "**/src/**" (cwd / "src" / "a" / "b.c"),    # true
  ]
```

It is typically used as a filter predicate in a [list operation](../language/4-list-operations.md):

```pbb
[ |f| f for files if |f| pb.matches "*.c" f ]
```

## Path suffix operators

File suffixes are rewritten using the `+` and `-` operators on a path, rather than a dedicated builtin:

- `path + string` appends `string` to the last path element.
- `path - string` removes `string` from the end of the last path element, and fails if it isn't a suffix.

```pbb
let
  source = cwd / "src" / "main.c";
  object = (source - ".c") + ".o";
in
  object
```

## Builtin build functions

### `pb.rule`

Creates a build-rule function describing how a build step should be performed. A rule captures the relevant inputs, outputs, and execution behavior for a single build action.

```pbb
compile_rule = pb.rule (|{input, output, ...}| {
  command = ["gcc", "-c", "-o", output, input];
});
```

Note: In `pb.rule`, object matcher defaults (for example, `|{input ? fallback, ...}|`) are not supported.

The return value of `pb.rule` is callable. Call it with a build argument object to produce a build value; this is the only way to construct a build value, there is no separate `pb.build` builtin:

```pbb
compile_rule {
  input = [cwd / "src" / "main.c"];
  output = cwd / "build" / "main.o";
}
```

The optional variable `deps` adds any implicit dependencies to the build, and is not passed to the rule itself. It can be either a single file/build or a list of files/builds.

More information is available in the [builds](../language/6-build-rules.md) chapter.

## Debugging

The `dbg` builtin object is used for debugging expressions during evaluation.

### `dbg.trace`

Attempts to evaluate the input expression, prints it, and returns it unchanged.

Syntax:
```pbb
dbg.trace expr
```

Behavior:
- First runs `eval` on `expr`. Any eval error is ignored.
- Prints `expr` using standard output.
- Returns the same expression value, so it can be inserted into larger
  expressions without changing behavior.

Example:
```pbb
|{dbg, ...}|
let
    x = dbg.trace (1 + 2);
in
x * 10
```

This prints `3` and evaluates to `30`.

### `dbg.break`

Attempts to evaluate the input expression, prints it, and then raises a debug
exception.

Syntax:
```pbb
dbg.break expr
```

Behavior:
- First runs `eval` on `expr`. Any eval error is ignored.
- Prints `expr` using standard output.
- Raises a `Debug` error with the message `break`.

Use `dbg.break` when you want evaluation to stop at a specific point and show
the current value.

Example:
```pbb
|{dbg, ...}|
let
    x = 1 + 2;
in
dbg.break x
```
