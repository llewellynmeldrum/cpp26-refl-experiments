# mylib

A C++ library template: CMake presets driven by a Makefile, tests, a header
self-containment check, install rules that produce a proper CMake package, and
a consumer project that proves the package works.

## Starting a new library

1. On GitHub, create a repo from this template ("Use this template"), clone it.
2. `./scripts/init.sh <name>` renames `mylib` everywhere (files, dirs, targets).
3. Set `project(<name> VERSION 0.1.0 ...)` and the knobs at the top of `CMakeLists.txt`.
4. `make check`.

Header-only by default. Drop any `.cpp` into `src/` and it becomes a compiled
static library (or shared, with `-DBUILD_SHARED_LIBS=ON`). Nothing else changes.

## Make targets

| Target | What it does |
|---|---|
| `make` / `make test` | dev build, run tests |
| `make test-<preset>` | same with `cfast`, `fast`, `asan`, `ausan` |
| `make run EX=basic` | build and run `examples/basic.cpp` |
| `make install` | release build, installed to `PREFIX` (default `~/.local`) |
| `make uninstall` | removes exactly the files the last `make install` wrote |
| `make package-test` | installs to `build/stage-<TC>`, builds `test_package/` against it with `find_package` |
| `make fetch-test` | builds `test_package/` pulling this checkout in with `FetchContent` |
| `make check` | `test-dev`, `test-ausan`, `package-test`, `fetch-test` |

`TC` picks a file from `cmake/toolchains/` (`native`, `gcc-16`, `llvm`). Build
dirs are `build/<TC>-<preset>`, so toolchains never share a cache. Put
`export TC=gcc-16` in your shell rc to make it the default.
`-B` and `-S` work as in the app Makefile.

## Layout

```
include/mylib/       public headers. Everything here is API.
src/                 optional .cpp files. Empty = header-only.
tests/               test_*.cpp -> one executable + one CTest test each
                     + an auto-generated "include each header alone" check
examples/            *.cpp -> one executable each. Write these first.
test_package/        a separate CMake project acting as a consumer
cmake/dev_flags.cmake   warnings, -Werror, sanitizers. Never reach consumers.
cmake/install.cmake     install + package config generation
cmake/Config.cmake.in   template for mylibConfig.cmake (list deps here)
cmake/toolchains/       compiler selection
```

## Using the library from another project

All three give the consumer the same target, `mylib::mylib`:

```cmake
# A. Installed package (after `make install`). Needs CMAKE_PREFIX_PATH=~/.local
#    unless ~/.local/bin is on PATH, which CMake also searches.
find_package(mylib 0.1 REQUIRED)

# B. Source, fetched at configure time and built as part of the consumer.
include(FetchContent)
FetchContent_Declare(mylib
    GIT_REPOSITORY https://github.com/llewellynmeldrum/mylib
    GIT_TAG        v0.1.0)
FetchContent_MakeAvailable(mylib)

# C. Source, vendored or a submodule.
add_subdirectory(external/mylib)

target_link_libraries(app PRIVATE mylib::mylib)
```

To develop the library and a consumer together, keep B in the consumer and
configure it with `-DFETCHCONTENT_SOURCE_DIR_MYLIB=/path/to/your/mylib/checkout`
(or put that in a local preset). The consumer then builds against your working
copy; fix the bug in the library repo, commit, tag, bump `GIT_TAG`.

## Adding a dependency

1. `find_package(dep REQUIRED)` (or `FetchContent_Declare(... FIND_PACKAGE_ARGS)`) in `CMakeLists.txt`.
2. `target_link_libraries(${PROJECT_NAME} ${_usage} dep::dep)`. Use `PRIVATE` instead if
   it is only used inside `src/*.cpp` and never appears in a public header.
3. Add `find_dependency(dep)` to `cmake/Config.cmake.in`.
4. `make check`. `package-test` fails if you forgot step 3.

## Releasing a version

1. Bump `project(... VERSION x.y.z)`.
2. `make check` (ideally with every `TC` you use).
3. `git tag vx.y.z && git push --tags`.

Consumers pin a tag. While at 0.x, every minor bump is allowed to break the API,
and the generated version file only accepts the same `0.minor`.
