# Builtin functions

The builtins are split into two categories:

- `ops` - operations, to access basic structures, like native objects and lists.
- `pb` - access lead-build related objects, like paths and builds.
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

Any missing index is padded with `null`

### `ops.transposeObjs`

Special case of `ops.transpose`, taking a list of objects, returing an object
of lists.

Difference from `ops.transpose` is that the inner lists are not padded with
null, and thus not maintaining the indexes.

```pbb
a = [
  { x = 1; y = 2; },
  { x = 3; z = 4; },
];

b = ops.transposeObjs;
```
produces:
```pbb
b = {
  x = [1, 3];
  y = [2];
  z = [4];
};
```

## Builtin path functions

### `pb.translate`

Rewrites a path by replacing a directory prefix.

Syntax:
```lead
pb.translate {
  input = path,
  from = path or [path, ...],
  to = path
}
```

- `input`: the path to rewrite
- `from`: one path or a list of candidate base path prefixes that may contain `input`
- `to`: the directory to use instead of `from`

Returns a path where the matching `from` prefix is removed from `input` and replaced by `to`.
If `from` is a list, the first matching prefix is used.

### `pb.rebase`

Converts a Lead path into a path value rebased to another filesystem base path.

Syntax:
```lead
pb.rebase {
  path = path,
  base = path
}
```

- `path`: the path value to convert
- `base`: the base directory to rebase the output against

Returns a path value that keeps the internal relative location of `path`, but expressed under `base`.

### `pb.lock`

Creates a new path value bound to the same file or directory, but with a fresh root boundary, so upward traversal (`..`) cannot escape above it.

Syntax:
```lead
pb.lock path
```

More information is available in the [paths](../language/5-paths.md) chapter.

## Path suffix operators

File suffixes are rewritten using the `+` and `-` operators on a path, rather than a dedicated builtin:

- `path + string` appends `string` to the last path element.
- `path - string` removes `string` from the end of the last path element, and fails if it isn't a suffix.

```lead
let
  source = cwd / "src" / "main.c";
  object = (source - ".c") + ".o";
in
  object
```

## Builtin build functions

### `pb.rule`

Creates a build-rule function describing how a build step should be performed. A rule captures the relevant inputs, outputs, and execution behavior for a single build action.

```lead
pb.rule |{input, output, ...}| {
  name = "compile";
  command = ["gcc", "-c", "-o", output, input];
};
```

Note: In `pb.rule`, object matcher defaults (for example, `|{input ? fallback, ...}|`) are not supported.

The return value of `pb.rule` is callable. Call it with a build argument object to produce a build value; this is the only way to construct a build value, there is no separate `pb.build` builtin:

```lead
compile_rule {
  input = [cwd / "src" / "main.c"];
  output = cwd / "build" / "main.o";
}
```

The optional variable `deps` adds any implicit dependencies to the build, and is not passed to the rule itself. It can be either a single file/build or a list of files/builds.

More information is available in the [builds](../language/6-build-rules.md) chapter.
# Debugging builtins

Lead Build exposes a `dbg` builtin object for debugging expressions during
evaluation.

## Debugging

### `dbg.trace`

Attempts to evaluate the input expression, prints it, and returns it unchanged.

Syntax:
```lead
dbg.trace expr
```

Behavior:
- First runs `eval` on `expr`. Any eval error is ignored.
- Prints `expr` using standard output.
- Returns the same expression value, so it can be inserted into larger
- expressions without changing behavior.

Example:
```lead
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
```lead
dbg.break expr
```

Behavior:
- First runs `eval` on `expr`. Any eval error is ignored.
- Prints `expr` using standard output.
- Raises a `Debug` error with the message `break`.

Use `dbg.break` when you want evaluation to stop at a specific point and show
the current value.

Example:
```lead
|{dbg, ...}|
let
    x = 1 + 2;
in
dbg.break x
```
