# expected_fs

`expected_fs` is a header-only C++23-or-newer wrapper around `std::filesystem`
that returns `std::expected<T, std::error_code>` by default instead of throwing
`std::filesystem_error` for ordinary filesystem failures.

## Requirements

* CMake 3.25+
* A C++23-or-newer compiler and standard library with `<expected>` support

C++20 is not supported in v1. Configuring or compiling with a C++20-only
toolchain will fail with a clear diagnostic. By default, CMake selects C++26
when the compiler exposes `cxx_std_26`; otherwise it falls back to C++23. Set
`expected_fs_CXX_STANDARD` to `23` or `26` to pin that choice explicitly. Other
values, or a requested standard unsupported by the compiler, fail during
configuration. C++26-only filesystem facilities are enabled only when the
standard library advertises the corresponding feature-test macro.

## Usage

```cpp
#include <expected_fs/expected_fs.hpp>

#include <filesystem>
#include <iostream>

int main() {
  const auto size = expected_fs::file_size("data.txt");
  if (!size) {
    std::cerr << size.error().message() << '\n';
    return 1;
  }

  std::cout << *size << '\n';
}
```

Most functions mirror the corresponding `std::filesystem` names inside the
`expected_fs` namespace:

```cpp
auto created = expected_fs::create_directories("out/cache");
auto copied = expected_fs::copy_file("input.txt", "out/input.txt");
auto removed = expected_fs::remove("out/input.txt");
```

The public expected alias mirrors `std::expected`, defaulting the error type to
`std::error_code` for filesystem wrappers:

```cpp
template <class T, class E = std::error_code>
using expected = std::expected<T, E>;
```

All 56 filesystem wrapper names declared directly in `expected_fs` are
customization point objects (CPOs). Together they cover 94 standard-compatible
call forms: 72 fallible forms and 22 non-fallible forms. A fallible call such as
`expected_fs::file_size(p)` uses the built-in `expected_fs::std_fs` filesystem
backend and `expected_fs::std_result` result domain by default, while explicit
backend, value, error, and result-domain calls are also supported:

```cpp
auto size = expected_fs::file_size("data.txt");
auto typed = expected_fs::file_size(expected_fs::with_value<my_size>,
                                    "data.txt");
auto mapped = expected_fs::file_size(expected_fs::with_error<my_error>,
                                    "data.txt");
auto typed_mapped =
    expected_fs::file_size(expected_fs::with_error<my_error>(
                               expected_fs::with_value<my_size>),
                           "data.txt");
auto custom_result = expected_fs::file_size(expected_fs::with_result(my_result),
                                           "data.txt");
auto custom = expected_fs::file_size(my_fs, my_path);
```

Fallible forms produce a result-domain value and support `with_value`,
`with_error`, and `with_result`. Non-fallible forms, such as
`expected_fs::status_known(status)` and iterator observers, return their raw
value; result adapters do not apply to them.

These names are function objects, not ordinary free functions. Existing call
expressions such as `expected_fs::file_size(p)` remain valid, but code that
takes a function address, stores a specific overload in a function pointer, or
otherwise depends on a free-function overload set must be updated. Moving these
APIs from free functions to customization point objects is therefore a
source-compatibility change even when ordinary calls are unchanged.

### Custom Filesystem Backends

There are three separate backend customization mechanisms:

* A direct operation override provides `tag_invoke` for an operation tag, such
  as `expected_fs::file_size_t`, and the backend's arguments. This override owns
  the complete operation contract, including its return type. It is not
  automatically rewritten by `with_value`, `with_error`, or `with_result`.
* A policy-aware backend provides `tag_invoke` for
  `expected_fs::result_adaptor<Fs, Policy>`. It returns
  `Policy::result<T>` and constructs outcomes through `Policy::success` and
  `Policy::failure`. This is the customization to implement when the backend
  should participate in the fallible `with_*` result adaptations. It does not
  apply to non-fallible forms.
* A traits backend specializes `expected_fs::fs_traits<Fs>` with its associated
  filesystem types and an `ops` type. Each operation implemented by `ops`
  becomes the fallback for that CPO. Fallible primitives participate in every
  `with_*` result adaptation; non-fallible primitives return their raw result.
  Backends may implement the protocol incrementally: only the primitive needed
  by a call is required.

Every `ops` primitive receives the stored backend as its first argument.
Fallible primitives additionally receive a `std::error_code&` as their last
argument; non-fallible primitives do not. This makes the same protocol suitable
for stateless adapters, in-memory filesystems, remote clients, and stateful test
doubles. For example, a backend that initially supports only `file_size` can be
defined as follows:

```cpp
struct my_ops {
  static std::uintmax_t file_size(my_fs& fs, const my_path& value,
                                  std::error_code& error);
};

template <>
struct expected_fs::fs_traits<my_fs> {
  using ops = my_ops;
  using path = my_path;
};
```

The complete protocol mirrors all 94 `expected_fs` wrapper forms, including
path queries and mutations, status predicates, `directory_entry` operations,
and both iterator types. Associated enum types supply the convenience-overload
defaults (`none` for copy/directory options and `replace` for permission
options). Explicit direct `tag_invoke` customizations always take precedence
over the traits fallback; policy-aware customizations do so for fallible forms.

The backend-first parameter is part of the protocol. An older experimental
primitive such as `Ops::file_size(path, error)` must be migrated to
`Ops::file_size(fs, path, error)`.

For a custom associated type, the explicit backend is also required to reach
the traits fallback: call `expected_fs::status_known(fs, status)`, for example,
not `expected_fs::status_known(status)`. The library does not try to infer `Fs`
by reverse-matching `status` against `fs_file_status_t<Fs>`. The same rule
applies to every non-fallible associated-value form.

Backends that expose both raw status predicates and fallible path or
`directory_entry` predicates must use a `file_status` type distinct from their
`path` and `directory_entry` types. Otherwise calls such as `exists(fs, value)`
cannot distinguish the raw and fallible forms; an exact `file_status` match is
treated as the raw form.

When the first argument has an `fs_traits` specialization, it is always treated
as an explicit backend. If that backend does not implement the requested
primitive, the expression is ill-formed; it never falls through by converting
the backend into an argument for `std_fs`.

The default `expected_fs::path`, `expected_fs::directory_iterator`, and related
aliases are derived from `fs_traits<expected_fs::std_fs_t>`, while
`expected_fs::fs_path_t<Fs>` and the other `fs_*_t` aliases expose the types
associated with an explicit backend. Result adapters preserve those associated
types and only change the result-domain, value, and error policy.
`with_value<void>` discards the natural success value while preserving errors.

Result adapters created from a backend own a decayed copy of that backend by
default, including when the source backend is an lvalue. Use `std::ref(fs)` or
`std::cref(fs)` explicitly to create a borrowing adapter instead:

```cpp
#include <functional>

auto owned = expected_fs::with_error<my_error>(my_fs);
auto borrowed = expected_fs::with_error<my_error>(std::ref(my_fs));
auto borrowed_const = expected_fs::with_error<my_error>(std::cref(my_fs));
```

A borrowed backend must outlive the adapter and every operation performed
through it. Borrowing does not add synchronization: the caller remains
responsible for the backend's thread-safety and for coordinating shared mutable
state.

### Built-in Result Domains

The optional `tl::expected` adapter provides a ready-made result domain without
adding `tl::expected` as a dependency of the core header:

```cpp
#include <expected_fs/result_domains/tl_expected.hpp>

inline constexpr auto tl_fs = expected_fs::with_result(
    expected_fs::tl_expected_result, expected_fs::std_fs);

auto size = expected_fs::file_size(tl_fs, "data.txt");
auto copied = expected_fs::copy_file(tl_fs, "a.txt", "b.txt");
```

Both results above use `tl::expected`. Calls without the explicit `tl_fs`
adaptor continue to use `std::expected`, so including the adapter never changes
the program-wide default.

The consumer must make `tl::expected` available. Its CMake package exports the
`tl::expected` target:

```cmake
find_package(tl-expected CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE
  expected_fs::expected_fs
  tl::expected
)
```

### Custom Result Domains

Third-party expected implementations can be integrated through a minimal,
stateless result-domain protocol:

* specialize `expected_fs::result_domain_traits<Domain>` with a
  `result<T, E>` alias;
* provide `tag_invoke` overloads for `expected_fs::result_success_t`, including
  the `void` success form; and
* provide a `tag_invoke` overload for `expected_fs::result_failure_t`.

No result observer hooks are required. A domain is a type-level tag: it must be
default-constructible, and state from an object passed to `with_result` is not
retained. Use `expected_fs::with_policy<Policy>` as the low-level escape hatch
for unusual result types or behavior that does not fit this stateless protocol.

## Header-Only Use

The `expected_fs` core is distributed as a self-contained public header. For
the simplest integration path, copy `include/expected_fs/expected_fs.hpp` into
your project and include it directly:

```cpp
#include <expected_fs/expected_fs.hpp>
```

The core header requires C++23 `<expected>` support and does not require
linking an additional library. Optional result-domain adapters are distributed
as separate headers and declare their own third-party include requirements.

## API Coverage

Coverage is tracked against the cppreference filesystem index and the linked
`path`, `directory_entry`, `directory_iterator`, and
`recursive_directory_iterator` pages:
<https://en.cppreference.com/w/cpp/filesystem>.

All 56 directly named `expected_fs` filesystem wrappers in the table are CPOs;
they represent 94 forms (72 fallible and 22 non-fallible). Rows marked as
aliases instead expose an associated concrete type, so its native members,
operators, and ADL functions remain available. Those concrete-type APIs are not
part of the named-wrapper CPO protocol and are not counted in the 56 names.
Fallible constructors cannot return `expected`, so they use `make_*` factory
functions.
C++26-only rows are feature-gated; `std::formatter<std::filesystem::path>` is
available through `expected_fs::path` when `EXPECTED_FS_HAS_FORMAT_PATH` is `1`
(`__cpp_lib_format_path >= 202403L`), and the same condition is exposed as
`expected_fs::has_format_path`.

| std | expected_fs | Status |
| --- | --- | --- |
| `std::filesystem::path` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::format`, `native_format`, `generic_format`, `auto_format` | `expected_fs::path::...` alias | ✅ |
| `std::filesystem::path` member types/constants | `expected_fs::path` alias | ✅ |
| `std::filesystem::path` constructors, destructor, assignment | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::assign` | `expected_fs::path::assign` alias | ✅ |
| `std::filesystem::path::append`, `operator/=` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::concat`, `operator+=` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::clear`, `make_preferred`, `remove_filename`, `replace_filename`, `replace_extension`, `swap` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::native`, `c_str`, `operator string_type` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::string`, `wstring`, `u8string`, `u16string`, `u32string` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::generic_string`, `generic_wstring`, `generic_u8string`, `generic_u16string`, `generic_u32string` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::compare` | `expected_fs::path::compare` alias | ✅ |
| `std::filesystem::path::lexically_normal`, `lexically_relative`, `lexically_proximate` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path` decomposition members | `expected_fs::path` alias | ✅ |
| `std::filesystem::path` query members | `expected_fs::path` alias | ✅ |
| `std::filesystem::path::begin`, `end` | `expected_fs::path` alias | ✅ |
| `std::filesystem::path` comparison operators | `expected_fs::path` alias | ✅ |
| `std::filesystem::operator/(path, path)` | `expected_fs::path` alias / ADL | ✅ |
| `std::filesystem::operator<<`, `operator>>` for `path` | `expected_fs::path` alias / ADL | ✅ |
| `std::filesystem::swap(path&, path&)` | `expected_fs::swap(path&, path&)` | ✅ |
| `std::filesystem::hash_value(path)` | `expected_fs::hash_value(path)` | ✅ |
| `std::filesystem::u8path` | `expected_fs::u8path` | ✅ |
| `std::hash<std::filesystem::path>` | `std::hash<expected_fs::path>` alias | ✅ |
| `std::formatter<std::filesystem::path>` | `std::formatter<expected_fs::path>` when `EXPECTED_FS_HAS_FORMAT_PATH` is `1` | ✅ |
| `std::filesystem::filesystem_error` | `expected_fs::filesystem_error` alias | ✅ |
| `std::filesystem::directory_entry` | `expected_fs::directory_entry` alias | ✅ |
| `std::filesystem::directory_entry` constructors | `expected_fs::make_directory_entry` for fallible construction; alias otherwise | ✅ |
| `std::filesystem::directory_entry::operator=` | `expected_fs::directory_entry` alias | ✅ |
| `std::filesystem::directory_entry::assign` | `expected_fs::assign(directory_entry&, path)` | ✅ |
| `std::filesystem::directory_entry::replace_filename` | `expected_fs::replace_filename(directory_entry&, path)` | ✅ |
| `std::filesystem::directory_entry::refresh` | `expected_fs::refresh(directory_entry&)` | ✅ |
| `std::filesystem::directory_entry::path`, `operator const path&` | `expected_fs::directory_entry` alias | ✅ |
| `std::filesystem::directory_entry::exists` | `expected_fs::exists(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::is_block_file` | `expected_fs::is_block_file(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::is_character_file` | `expected_fs::is_character_file(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::is_directory` | `expected_fs::is_directory(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::is_fifo` | `expected_fs::is_fifo(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::is_other` | `expected_fs::is_other(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::is_regular_file` | `expected_fs::is_regular_file(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::is_socket` | `expected_fs::is_socket(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::is_symlink` | `expected_fs::is_symlink(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::file_size` | `expected_fs::file_size(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::hard_link_count` | `expected_fs::hard_link_count(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::last_write_time` | `expected_fs::last_write_time(directory_entry)` | ✅ |
| `std::filesystem::directory_entry::status`, `symlink_status` | `expected_fs::status(directory_entry)`, `expected_fs::symlink_status(directory_entry)` | ✅ |
| `std::filesystem::directory_entry` comparison operators | `expected_fs::directory_entry` alias | ✅ |
| `std::filesystem::operator<<` for `directory_entry` | `expected_fs::directory_entry` alias / ADL | ✅ |
| `std::filesystem::directory_iterator` | `expected_fs::directory_iterator` alias | ✅ |
| `std::filesystem::directory_iterator` constructors | `expected_fs::make_directory_iterator` for fallible construction; alias otherwise | ✅ |
| `std::filesystem::directory_iterator::operator*`, `operator->`, `operator=` | `expected_fs::directory_iterator` alias | ✅ |
| `std::filesystem::directory_iterator::increment`, `operator++` | `expected_fs::increment(directory_iterator&)` for fallible increment; alias otherwise | ✅ |
| `std::filesystem::begin/end(directory_iterator)` | `expected_fs::begin/end(directory_iterator)` | ✅ |
| `std::ranges` iterator helper specializations for `directory_iterator` | `expected_fs::directory_iterator` alias | ✅ |
| `std::filesystem::recursive_directory_iterator` | `expected_fs::recursive_directory_iterator` alias | ✅ |
| `std::filesystem::recursive_directory_iterator` constructors | `expected_fs::make_recursive_directory_iterator` for fallible construction; alias otherwise | ✅ |
| `std::filesystem::recursive_directory_iterator::operator*`, `operator->`, `operator=` | `expected_fs::recursive_directory_iterator` alias | ✅ |
| `std::filesystem::recursive_directory_iterator::options` | `expected_fs::options(recursive_directory_iterator)` | ✅ |
| `std::filesystem::recursive_directory_iterator::depth` | `expected_fs::depth(recursive_directory_iterator)` | ✅ |
| `std::filesystem::recursive_directory_iterator::recursion_pending` | `expected_fs::recursion_pending(recursive_directory_iterator)` | ✅ |
| `std::filesystem::recursive_directory_iterator::increment`, `operator++` | `expected_fs::increment(recursive_directory_iterator&)` for fallible increment; alias otherwise | ✅ |
| `std::filesystem::recursive_directory_iterator::pop` | `expected_fs::pop(recursive_directory_iterator&)` | ✅ |
| `std::filesystem::recursive_directory_iterator::disable_recursion_pending` | `expected_fs::disable_recursion_pending(recursive_directory_iterator&)` | ✅ |
| `std::filesystem::begin/end(recursive_directory_iterator)` | `expected_fs::begin/end(recursive_directory_iterator)` | ✅ |
| `std::ranges` iterator helper specializations for `recursive_directory_iterator` | `expected_fs::recursive_directory_iterator` alias | ✅ |
| `std::filesystem::file_status` | `expected_fs::file_status` alias | ✅ |
| `std::filesystem::space_info` | `expected_fs::space_info` alias | ✅ |
| `std::filesystem::file_type` | `expected_fs::file_type` alias | ✅ |
| `std::filesystem::file_time_type` | `expected_fs::file_time_type` alias | ✅ |
| `std::filesystem::perms` | `expected_fs::perms` alias | ✅ |
| `std::filesystem::perm_options` | `expected_fs::perm_options` alias | ✅ |
| `std::filesystem::copy_options` | `expected_fs::copy_options` alias | ✅ |
| `std::filesystem::directory_options` | `expected_fs::directory_options` alias | ✅ |
| `std::filesystem::absolute` | `expected_fs::absolute` | ✅ |
| `std::filesystem::canonical` | `expected_fs::canonical` | ✅ |
| `std::filesystem::weakly_canonical` | `expected_fs::weakly_canonical` | ✅ |
| `std::filesystem::relative` | `expected_fs::relative` | ✅ |
| `std::filesystem::proximate` | `expected_fs::proximate` | ✅ |
| `std::filesystem::copy` | `expected_fs::copy` | ✅ |
| `std::filesystem::copy_file` | `expected_fs::copy_file` | ✅ |
| `std::filesystem::copy_symlink` | `expected_fs::copy_symlink` | ✅ |
| `std::filesystem::create_directory` | `expected_fs::create_directory` | ✅ |
| `std::filesystem::create_directories` | `expected_fs::create_directories` | ✅ |
| `std::filesystem::create_hard_link` | `expected_fs::create_hard_link` | ✅ |
| `std::filesystem::create_symlink` | `expected_fs::create_symlink` | ✅ |
| `std::filesystem::create_directory_symlink` | `expected_fs::create_directory_symlink` | ✅ |
| `std::filesystem::current_path` | `expected_fs::current_path` | ✅ |
| `std::filesystem::temp_directory_path` | `expected_fs::temp_directory_path` | ✅ |
| `std::filesystem::exists` | `expected_fs::exists` | ✅ |
| `std::filesystem::equivalent` | `expected_fs::equivalent` | ✅ |
| `std::filesystem::file_size` | `expected_fs::file_size` | ✅ |
| `std::filesystem::hard_link_count` | `expected_fs::hard_link_count` | ✅ |
| `std::filesystem::last_write_time` | `expected_fs::last_write_time` | ✅ |
| `std::filesystem::permissions` | `expected_fs::permissions` | ✅ |
| `std::filesystem::read_symlink` | `expected_fs::read_symlink` | ✅ |
| `std::filesystem::remove` | `expected_fs::remove` | ✅ |
| `std::filesystem::remove_all` | `expected_fs::remove_all` | ✅ |
| `std::filesystem::rename` | `expected_fs::rename` | ✅ |
| `std::filesystem::resize_file` | `expected_fs::resize_file` | ✅ |
| `std::filesystem::space` | `expected_fs::space` | ✅ |
| `std::filesystem::status` | `expected_fs::status` | ✅ |
| `std::filesystem::symlink_status` | `expected_fs::symlink_status` | ✅ |
| `std::filesystem::is_block_file` | `expected_fs::is_block_file` | ✅ |
| `std::filesystem::is_character_file` | `expected_fs::is_character_file` | ✅ |
| `std::filesystem::is_directory` | `expected_fs::is_directory` | ✅ |
| `std::filesystem::is_empty` | `expected_fs::is_empty` | ✅ |
| `std::filesystem::status_known` | `expected_fs::status_known` | ✅ |
| `std::filesystem::is_fifo` | `expected_fs::is_fifo` | ✅ |
| `std::filesystem::is_other` | `expected_fs::is_other` | ✅ |
| `std::filesystem::is_regular_file` | `expected_fs::is_regular_file` | ✅ |
| `std::filesystem::is_socket` | `expected_fs::is_socket` | ✅ |
| `std::filesystem::is_symlink` | `expected_fs::is_symlink` | ✅ |

## Building And Testing

```bash
cmake -S . -B build \
  -DCMAKE_INSTALL_PREFIX=/absolute/path/to/install \
  -DCPM_SOURCE_CACHE=$HOME/.cache/CPM \
  -Dexpected_fs_CXX_STANDARD=23

cmake --build build
ctest --test-dir build -C Debug -VV
cmake --build build --target install
```

To configure without tests:

```bash
cmake -S . -B build \
  -Dexpected_fs_ENABLE_UNIT_TESTING=OFF \
  -DCMAKE_INSTALL_PREFIX=/absolute/path/to/install
```

## CMake Integration

After installation:

```cmake
find_package(expected_fs CONFIG REQUIRED)
target_link_libraries(your_target PRIVATE expected_fs::expected_fs)
```

Since this is header-only, linking the target only provides include directories,
and compile features.

### FetchContent

```cmake
include(FetchContent)

FetchContent_Declare(
  expected_fs
  GIT_REPOSITORY https://github.com/MrZLeo/expected-fs.git
  GIT_TAG v0.1.0
)
FetchContent_MakeAvailable(expected_fs)

target_link_libraries(your_target PRIVATE expected_fs::expected_fs)
```

### CPM.cmake

```cmake
CPMAddPackage(
  NAME expected_fs
  GITHUB_REPOSITORY MrZLeo/expected-fs
  GIT_TAG v0.1.0
)

target_link_libraries(your_target PRIVATE expected_fs::expected_fs)
```

## Development

Useful local shortcuts:

* `make test`
* `make coverage`
* `make docs`
* `make install`
* `make format`

The project uses Catch2 through CPM.cmake for tests.

## License

This project is licensed under the BSD 3-Clause. See [LICENSE](LICENSE) for
details.
