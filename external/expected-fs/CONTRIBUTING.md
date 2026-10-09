# Contributing To expected_fs

Thank you for helping improve `expected_fs`.

## Development Setup

Build and test from the repository root:

```bash
cmake -S . -B build -DCPM_SOURCE_CACHE=$PWD/.cpm-cache
cmake --build build
ctest --test-dir build -VV
```

The project requires C++23 or newer with `<expected>` support. C++26-only
standard library facilities must be guarded by their feature-test macros.

## Guidelines

* Keep changes focused and small enough to review.
* Add or update tests for behavior changes.
* Prefer thin wrappers around the non-throwing `std::filesystem` overloads.
* Preserve `std::error_code` as the default error channel. Custom error types
  selected explicitly through `with_error<E>` are a supported part of the
  public protocol; do not silently change the default result type.
* Keep result domains stateless and default-constructible. Their minimum
  protocol is `result_domain_traits`, `result_success_t`, and
  `result_failure_t`; do not require observer hooks without a demonstrated use.
* Treat backend ownership as part of the API contract. Result adapters own a
  decayed backend copy by default, while `std::ref` and `std::cref` opt into
  borrowing. Tests for borrowed adapters must cover lifetime assumptions and
  any relevant thread-safety constraints.
* Keep the backend dispatch order explicit: direct operation `tag_invoke`,
  policy-aware `result_adaptor<Fs, Policy>` customization, then the
  `fs_traits<Fs>::ops` fallback. Every fallible `ops` primitive receives the
  stored backend as its first argument and `std::error_code&` as its last.
  Detect primitives per operation so partial backends remain valid.
* Avoid adding runtime dependencies; this library is intended to remain
  header-only.

## Pull Requests

Use [PULL_REQUEST_TEMPLATE.md](PULL_REQUEST_TEMPLATE.md) and include:

* The problem being solved.
* The relevant API behavior.
* The commands used to build and test.
* Any compatibility concerns for C++23/C++26 compilers or standard libraries.
* Any source-compatibility impact from changing a fallible API customization
  point object, especially code that takes function addresses or depends on a
  free-function overload set.

To skip CI for documentation-only commits, include `[skip ci]` or `[ci skip]` in
the commit message.
