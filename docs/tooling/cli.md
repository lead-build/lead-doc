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
  -v, --verbose           Increase verbosity (can be passed multiple times, up to -vvv)
  -h, --help              Print help
  -V, --version           Print version
```

## pbfmt

A formatter and linter for Lead configuration files (.pbb).

```
Usage: pbfmt [OPTIONS] [FILE]...

Arguments:
  [FILE]...  The .pbb file(s) to format

Options:
  -s, --stdin   Read source from stdin instead of a file
  -l, --lint    Only check that the file parses, without reformatting
  -i, --in-place  Format the file in place instead of printing to stdout
  -h, --help    Print help
  -V, --version Print version
```

## pbls

The Lead Language Server for IDE integration and editor support.

```
Usage: pbls [OPTIONS]

Options:
  --stdio              Communicate over stdio
  --socket <PORT>      Communicate over a TCP socket
  --bind-host <HOST>   Host to bind the TCP socket to
  -h, --help           Print help
  -V, --version        Print version
```
