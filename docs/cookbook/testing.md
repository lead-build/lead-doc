# Running Tests

Build unit tests together with the code they test, run them, and collect the results in a test report.

**When to use:** When your project has tests that should be built from the same modules as the application.

## Example

```pbb
--8<-- "examples/cookbook/testing/main.pbb"
```

## How It Works

- `tests` in `lib.lang.c.mod` — Lists the test files of the module. Each test file is compiled and linked together with the objects of the build into its own executable, placed in `testcases` in the output directory
- `lib.test_report` — Takes the path of the report and the same list of modules as `lib.build`. It builds the tests, and generates a rule that runs all of them and writes the report

Since every test executable has its own `main()`, the module containing the `main()` of the application is kept out of the modules passed to the tests.

Each test file becomes a separate executable. Here, `test_add.c` and `test_mul.c` are each linked with the object of `utils.c`, one at a time, but never with each other:

```ninja
build out/testcases/tests/test_add: gcc_o build/src/utils.o build/tests/test_add.o
build out/testcases/tests/test_mul: gcc_o build/src/utils.o build/tests/test_mul.o
```

So every test can have its own `main()`, and a test crashing does not stop the others from running.

## Source Files

**src/utils.h:**
```c
--8<-- "examples/cookbook/testing/src/utils.h"
```

**src/utils.c:**
```c
--8<-- "examples/cookbook/testing/src/utils.c"
```

**src/main.c:**
```c
--8<-- "examples/cookbook/testing/src/main.c"
```

A test is an executable that passes if it exits with status 0, and fails otherwise:

**tests/test_add.c:**
```c
--8<-- "examples/cookbook/testing/tests/test_add.c"
```

**tests/test_mul.c:**
```c
--8<-- "examples/cookbook/testing/tests/test_mul.c"
```

## Running the Tests

Generate the build, and run the tests through the `test` alias:

```bash
pb
ninja test
```

The report is printed as the tests run, and written to `out/test_report.txt`:

```text
Running 2 test(s)

PASS  out/testcases/tests/test_add  (0.001s)
PASS  out/testcases/tests/test_mul  (0.001s)

2 test(s), 2 passed, 0 failed (0.005s)
```

The output of failed tests is included in the report. Generating the report succeeds even if tests fail, so evaluating the result is left to other tools.

## Test Fixtures

Tests often need code that should not be part of the application, such as a test framework, helpers, or stubs. Wrapping a module with `lib.lang.tests.for_test` makes it a test fixture: it is only added when building the tests, and does not affect the application.

```pbb
utils = lib.merge [
    lib.lang.c.mod {
        src = [ "${cwd}/src/utils.c" ];
        inc = [ "${cwd}/src/" ];
        tests = [ "${cwd}/tests/test_add.c" ];
    },
    lib.lang.tests.for_test (lib.lang.c.mod {
        src = [ "${cwd}/tests/check.c" ];
        inc = [ "${cwd}/tests/" ];
    }),
];
```

The fixture is part of the module, so it follows the module wherever it is used, and `utils` can still be passed to `lib.build` as before. Here, `tests/check.c` could implement a small check helper, declared in `tests/check.h`, available to all tests of the build.

## Multiple Configurations

To run the tests for several configurations, use `lib.test_multi_report`. It takes a list of module lists, one per configuration, and generates a single report with the tests from all of them:

```pbb
lib.test_multi_report "${cwd}/out/test_report.txt" [
    [ config_gcc, utils ],
    [ config_clang, utils ],
]
```

Each list of modules is combined on its own, just as if it was passed to `lib.test_report`, so each configuration builds its tests with its own settings and its own test fixtures. Only the report is shared. Make sure the configurations have different build and output directories, since they would otherwise write to the same files. See [Multiple Targets](multiple-targets.md) for how to set up the configurations.

In fact, `lib.test_report` is just `lib.test_multi_report` with a single list of modules:

```pbb
test_report = |output mods| test_multi_report output [mods];
```

## See Also

- [Simple C Application](simple-c-app.md) — Start here if you're new
- [Multiple Targets with Different Configurations](multiple-targets.md) — Building several configurations
