#include "expected_fs/result_domains/tl_expected.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <concepts>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <system_error>
#include <utility>

namespace {
namespace tl_expected_test {
struct mapped_error {
  std::error_code value;

  explicit mapped_error(std::error_code error) : value(error) {}
};

struct mapped_size {
  std::uintmax_t value{};

  explicit mapped_size(std::uintmax_t size) : value(size) {}
};
} // namespace tl_expected_test
} // namespace

namespace {
namespace fs = std::filesystem;

class temporary_directory {
public:
  temporary_directory() {
    const auto base = fs::temp_directory_path();
    const auto seed =
        std::chrono::steady_clock::now().time_since_epoch().count();

    for (int attempt = 0; attempt < 100; ++attempt) {
      const auto candidate = base
                           / ("expected_fs_tl_expected_test_"
                              + std::to_string(seed)
                              + "_"
                              + std::to_string(attempt));

      std::error_code error;
      if (fs::create_directory(candidate, error)) {
        path_ = candidate;
        return;
      }
    }

    throw std::runtime_error("failed to create temporary test directory");
  }

  temporary_directory(const temporary_directory &) = delete;
  temporary_directory &operator=(const temporary_directory &) = delete;
  temporary_directory(temporary_directory &&) = delete;
  temporary_directory &operator=(temporary_directory &&) = delete;

  ~temporary_directory() {
    std::error_code error;
    fs::remove_all(path_, error);
  }

  [[nodiscard]] const fs::path &path() const noexcept { return path_; }

private:
  fs::path path_;
};

void write_text(const fs::path &path, const std::string &contents) {
  std::ofstream stream(path, std::ios::binary);
  if (!stream) {
    throw std::runtime_error("failed to open test file for writing");
  }

  stream << contents;
}

using mapped_error = tl_expected_test::mapped_error;
using mapped_size = tl_expected_test::mapped_size;

inline constexpr auto tl_fs = expected_fs::with_result(
    expected_fs::tl_expected_result, expected_fs::std_fs);

static_assert(std::same_as<decltype(expected_fs::file_size(
                               std::declval<const fs::path &>())),
                           std::expected<std::uintmax_t, std::error_code>>);
static_assert(std::same_as<decltype(expected_fs::file_size(
                               tl_fs, std::declval<const fs::path &>())),
                           tl::expected<std::uintmax_t, std::error_code>>);
static_assert(std::same_as<decltype(expected_fs::copy(
                               tl_fs, std::declval<const fs::path &>(),
                               std::declval<const fs::path &>())),
                           tl::expected<void, std::error_code>>);
static_assert(std::same_as<decltype(expected_fs::file_size(
                               expected_fs::with_value<void>(tl_fs),
                               std::declval<const fs::path &>())),
                           tl::expected<void, std::error_code>>);
static_assert(std::same_as<decltype(expected_fs::file_size(
                               expected_fs::with_error<mapped_error>(
                                   expected_fs::with_value<mapped_size>(tl_fs)),
                               std::declval<const fs::path &>())),
                           tl::expected<mapped_size, mapped_error>>);
} // namespace

TEST_CASE("tl::expected result domain returns successful filesystem results",
          "[integration][tl-expected]") {
  const temporary_directory directory;
  const auto source = directory.path() / "source.txt";
  const auto destination = directory.path() / "destination.txt";
  write_text(source, "hello");

  const auto size = expected_fs::file_size(tl_fs, source);
  REQUIRE(size.has_value());
  REQUIRE(*size == std::uintmax_t{5});

  const auto mapped =
      expected_fs::file_size(expected_fs::with_error<mapped_error>(
                                 expected_fs::with_value<mapped_size>(tl_fs)),
                             source);
  REQUIRE(mapped.has_value());
  REQUIRE(mapped->value == std::uintmax_t{5});

  const auto discarded =
      expected_fs::file_size(expected_fs::with_value<void>(tl_fs), source);
  REQUIRE(discarded.has_value());

  const auto copied = expected_fs::copy(tl_fs, source, destination);
  REQUIRE(copied.has_value());
  REQUIRE(fs::exists(destination));
}

TEST_CASE("tl::expected result domain preserves filesystem errors",
          "[integration][tl-expected]") {
  const temporary_directory directory;
  const auto missing = directory.path() / "missing.txt";

  std::error_code expected_error;
  static_cast<void>(fs::file_size(missing, expected_error));
  REQUIRE(expected_error);

  const auto size = expected_fs::file_size(tl_fs, missing);
  REQUIRE_FALSE(size.has_value());
  REQUIRE(size.error() == expected_error);

  const auto mapped = expected_fs::file_size(
      expected_fs::with_error<mapped_error>(tl_fs), missing);
  REQUIRE_FALSE(mapped.has_value());
  REQUIRE(mapped.error().value == expected_error);

  const auto discarded =
      expected_fs::file_size(expected_fs::with_value<void>(tl_fs), missing);
  REQUIRE_FALSE(discarded.has_value());
  REQUIRE(discarded.error() == expected_error);

  const auto copied =
      expected_fs::copy(tl_fs, missing, directory.path() / "destination.txt");
  REQUIRE_FALSE(copied.has_value());
  REQUIRE(copied.error());
}
