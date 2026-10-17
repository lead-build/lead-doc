# Paths

Paths are values in the language that represent a location in the filesystem. A path value may refer to either a file or a directory.

Paths are typically obtained from builtin functions or other language constructs. The builtin `cwd` represents the directory of the current `.pbb` file and is commonly used as the starting point for path traversal. Every example below includes a file header that brings `cwd` into scope.

## Bound root and purity

To keep modules within a build pure, a module should not have side effects. This also needs to be true for the files it reads and generates. Therefore, a module, represented by the location of its `.pbb` file, can not access any other files than the ones within the module itself, unless paths are passed explicitly to the module.

To achieve this isolation, `cwd`, which is the path to the directory of the current `.pbb` file, is *locked* to that directory. Traversing downwards and upwards is allowed within a path, but never above the directory it is locked to, which is called its *origin*.

For the same reason, a path can not be created from nothing. It can only be created from an existing path by traversal.

Paths can be passed as arguments to functions, as all values. For example, it is possible to pass the path to a build directory to a function in another module, to allow that module to generate files in the build directory.

## Traversal

Use the `/` operator to move down into a directory or file name:

```pbb
|{cwd, ...}|
let
  src = cwd / "src";
  main = src / "main.c";
in
  main
```

In this example, `src` is the path one level below `cwd`, and `main` is a path below `src`.

The right-hand side of `/` must be a string representing a child name.

## Upward traversal

You can also move upward using the special segment `".."`:

```pbb
|{cwd, ...}|
let
  src = cwd / "src";
  back = src / "..";
in
  back
```

This returns the parent directory of `src`, which is `cwd` itself. Going one step further, above the original `cwd` origin, is an error.

## Locking a path

The builtin `pb` contains a function called `lock` that creates a new path value bound to the same file or directory, but with a fresh root boundary.

```pbb
|{cwd, pb, ...}|
let
  locked = pb.lock (cwd / "src");
  parent = locked / "..";
in
  parent
```

In this example, `locked` refers to the same directory as `cwd / "src"`, but its upward traversal is restricted to that path. Evaluating this example therefore fails, since `locked / ".."` would escape above the locked root.

## Path remapping

Use the builtin `pb.translate` to rewrite a path by replacing one directory prefix with another. The argument is an object with `input`, `from`, and `to` fields. `input` is the path to rewrite, `from` is either one base directory or a list of base directories that may contain `input`, and `to` is the directory that should replace the matching prefix.

```pbb
|{cwd, pb, ...}|
let
  src = cwd / "src" / "main.c";
  remapped = pb.translate {
    input = src;
    from = cwd / "src";
    to = cwd / "build";
  };
in
  remapped
```

This produces a path rooted at `cwd / "build" / "main.c"`, where the `src` prefix has been replaced by `build`.

When `from` is a list, `pb.translate` checks candidates in order and uses the first one that matches.

```pbb
|{cwd, pb, ...}|
let
  file = cwd / "lib" / "src" / "main.c";
  remapped = pb.translate {
    input = file;
    from = [cwd / "src", cwd / "lib" / "src"];
    to = cwd / "build";
  };
in
  remapped
```

## File suffix rewriting

File suffixes are rewritten using the `+` and `-` operators, rather than a dedicated builtin. `path + string` appends `string` to the last path element, and `path - string` removes `string` from the end of the last path element (failing if it isn't a suffix of it).

```pbb
|{cwd, ...}|
let
  source = cwd / "src" / "main.c";
  object = (source - ".c") + ".o";
in
  object
```

This rewrites `main.c` to `main.o`.

## Matching paths against patterns

Use the builtin `pb.matches` to test whether a path matches a glob pattern. It takes the pattern first and the path second, and returns a boolean. Partially applying the pattern gives a reusable predicate, which works well together with the filter of a [map expression](4-list-operations.md).

```pbb
|{cwd, pb, ...}|
let
  files = [cwd / "src" / "main.c", cwd / "src" / "main.h", cwd / "src" / "util.c"];
  is_c = pb.matches "*.c";
in
  [ |f| f for files if |f| is_c f ]
```

A pattern without a `/`, such as `*.c`, is compared with the file name only. A pattern with a `/` is compared with the whole path, where `*` does not cross directory boundaries, and `**` does: `**/src/*.c` matches any `.c` file directly in a directory named `src`. See the [builtin reference](../language-reference/builtins.md#pbmatches) for the full pattern syntax.

## Rebase paths

Use the builtin `pb.rebase` to express a path relative to another base path. The result is not a path, but a string.

```pbb
|{cwd, pb, ...}|
let
  src = cwd / "src" / "main.c";
  out = pb.rebase {
    path = src;
    base = cwd / "build";
  };
in
  out
```

This evaluates to the string `"../src/main.c"`, which is how to reach `src` from the directory `build`.

This is useful when handing paths to tools that are run from another directory than the one where the build is run.

