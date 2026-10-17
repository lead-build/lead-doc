# Command Line Interface

Lead provides three main command-line tools for working with build configurations.

## pb

The main Lead compiler that generates Ninja build files from your Lead configuration.

```
Usage: pb [OPTIONS]

Options:
  -i, --input <INPUT>     Root description file
  -o, --output <OUTPUT>   Output file path
  -C <DIR>                Change directory before invoking command
  -P, --path <NAME=PATH>  Add paths to the top level args object
  -E, --eval              Print evaluated output instead of generating build.ninja
  -v, --verbose...        Increase verbosity (can be passed multiple times, up to -vvv)
  -h, --help              Print help
  -V, --version           Print version
```

Without `-o`, the output is written to `build.ninja`.

With `-E`, nothing is generated. Instead the evaluated result is printed, which is useful for trying out the language and for debugging. See the [language guide](../language/index.md#evaluation-and-testing).

## pbfmt

A formatter and linter for Lead configuration files (`.pbb`).

```
Usage: pbfmt [OPTIONS] [FILE]...

Arguments:
  [FILE]...  The .pbb file(s) to format (required unless --stdin is given).
             Multiple files are only allowed together with --in-place

Options:
  -s, --stdin     Read the source from stdin instead of a file
  -l, --lint      Only check that the file parses, without reformatting it
  -i, --in-place  Format the file in place instead of printing to stdout
  -h, --help      Print help
  -V, --version   Print version
```

Comments are kept by the formatter. So are blank lines between expressions, but runs of more than two blank lines are collapsed to two, and blank lines at the start and end of a file are removed.

## pbls

The Lead language server, for IDE integration and editor support. It is what the [VS Code extension](vscode.md) uses, and can be used from any editor supporting the Language Server Protocol.

```
Usage: pbls [OPTIONS]

Options:
  -h, --help     Print help
  -V, --version  Print version

Transport:
      --stdio             Communicate over stdio
      --socket <PORT>     Communicate over a TCP socket bound to this port on localhost
      --bind-host <HOST>  Host to bind the TCP socket to. Only valid together with --socket
```
