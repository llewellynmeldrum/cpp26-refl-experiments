#include "expected_fs/expected_fs.hpp"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <filesystem>
#include <functional>
#if EXPECTED_FS_HAS_FORMAT_PATH
#include <format>
#endif
#include <fstream>
#include <ios>
#include <iterator>
#include <set>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>

namespace {
namespace test_backend {
struct std_like_path {
  bool missing{};

  // Keep both backend and path convertible to the standard path type so the
  // partial-backend tests exercise fail-closed dispatch rather than a simple
  // type mismatch.
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator std::filesystem::path() const { return "custom-path"; }
};

struct std_like_fs {
  // A backend must never be reinterpreted as a default std_fs path when one of
  // its partial ops is unavailable.
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator std::filesystem::path() const { return "backend-as-path"; }
};

struct std_like_ops {
  [[nodiscard]] static std::uintmax_t
  file_size(const std_like_fs &, const std_like_path &value,
            std::error_code &error) {
    if (value.missing) {
      error = std::make_error_code(std::errc::no_such_file_or_directory);
      return static_cast<std::uintmax_t>(-1);
    }

    error.clear();
    return 512;
  }
};

struct convertible_size {
  std::uintmax_t value{};

  // Intentionally implicit: this exercises the backend's convertible_to
  // contract rather than an exact return type.
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  [[nodiscard]] operator std::uintmax_t() const noexcept { return value; }
};

struct convertible_fs {};

struct convertible_ops {
  [[nodiscard]] static convertible_size
  file_size(const convertible_fs &, const std_like_path &value,
            std::error_code &error) {
    if (value.missing) {
      error = std::make_error_code(std::errc::no_such_file_or_directory);
      return convertible_size{static_cast<std::uintmax_t>(-1)};
    }

    error.clear();
    return convertible_size{768};
  }
};

struct tracking_entry {
  std::filesystem::path value;
  int refresh_count{};
};

struct tracking_directory_iterator {
  int position{};
};

struct tracking_recursive_directory_iterator {
  int position{};
};

struct raw_path {
  std::string value;
};

enum class raw_file_type : std::uint8_t {
  unknown,
  missing,
  block,
  character,
  directory,
  fifo,
  other,
  regular,
  socket,
  symlink,
};

struct raw_status {
  raw_file_type type{raw_file_type::unknown};
};

struct raw_directory_iterator {
  int position{};
};

struct raw_recursive_directory_iterator {
  int position{};
  std::filesystem::directory_options iterator_options{
      std::filesystem::directory_options::none};
  int iterator_depth{};
  bool pending{true};
};

struct raw_fs {
  std::string prefix{"raw:"};
  std::size_t hash_seed{100};
  int call_count{};
};

struct raw_ops {
  template <class Source>
  [[nodiscard]] static raw_path u8path(raw_fs &backend, Source &&source) {
    ++backend.call_count;
    return raw_path{backend.prefix + std::string{std::forward<Source>(source)}};
  }

  template <std::input_iterator InputIterator>
  [[nodiscard]] static raw_path
  u8path(raw_fs &backend, InputIterator first, InputIterator last) {
    ++backend.call_count;
    return raw_path{backend.prefix + std::string{first, last}};
  }

  static void swap(raw_fs &backend, raw_path &left, raw_path &right) noexcept {
    ++backend.call_count;
    left.value.swap(right.value);
  }

  [[nodiscard]] static std::size_t
  hash_value(raw_fs &backend, const raw_path &value) noexcept {
    ++backend.call_count;
    return backend.hash_seed + value.value.size();
  }

  [[nodiscard]] static bool
  status_known(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type != raw_file_type::unknown;
  }

  [[nodiscard]] static bool exists(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type != raw_file_type::unknown
        && value.type != raw_file_type::missing;
  }

  [[nodiscard]] static bool
  is_block_file(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type == raw_file_type::block;
  }

  [[nodiscard]] static bool
  is_character_file(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type == raw_file_type::character;
  }

  [[nodiscard]] static bool
  is_directory(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type == raw_file_type::directory;
  }

  [[nodiscard]] static bool
  is_fifo(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type == raw_file_type::fifo;
  }

  [[nodiscard]] static bool
  is_other(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type == raw_file_type::other;
  }

  [[nodiscard]] static bool
  is_regular_file(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type == raw_file_type::regular;
  }

  [[nodiscard]] static bool
  is_socket(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type == raw_file_type::socket;
  }

  [[nodiscard]] static bool
  is_symlink(raw_fs &backend, raw_status value) noexcept {
    ++backend.call_count;
    return value.type == raw_file_type::symlink;
  }

  [[nodiscard]] static raw_directory_iterator
  begin(raw_fs &backend, raw_directory_iterator iterator) noexcept {
    ++backend.call_count;
    ++iterator.position;
    return iterator;
  }

  [[nodiscard]] static raw_directory_iterator
  end(raw_fs &backend, raw_directory_iterator) noexcept {
    ++backend.call_count;
    return raw_directory_iterator{-1};
  }

  [[nodiscard]] static raw_recursive_directory_iterator
  begin(raw_fs &backend, raw_recursive_directory_iterator iterator) noexcept {
    ++backend.call_count;
    iterator.position += 2;
    return iterator;
  }

  [[nodiscard]] static raw_recursive_directory_iterator
  end(raw_fs &backend, raw_recursive_directory_iterator) noexcept {
    ++backend.call_count;
    return raw_recursive_directory_iterator{.position = -2};
  }

  [[nodiscard]] static std::filesystem::directory_options
  options(raw_fs &backend,
          const raw_recursive_directory_iterator &iterator) noexcept {
    ++backend.call_count;
    return iterator.iterator_options;
  }

  [[nodiscard]] static int
  depth(raw_fs &backend,
        const raw_recursive_directory_iterator &iterator) noexcept {
    ++backend.call_count;
    return iterator.iterator_depth;
  }

  [[nodiscard]] static bool
  recursion_pending(raw_fs &backend,
                    const raw_recursive_directory_iterator &iterator) noexcept {
    ++backend.call_count;
    return iterator.pending;
  }

  static void disable_recursion_pending(
      raw_fs &backend, raw_recursive_directory_iterator &iterator) noexcept {
    ++backend.call_count;
    iterator.pending = false;
  }
};

struct partial_raw_fs {
  int hash_calls{};

  // Keep the backend convertible to path so missing operations specifically
  // prove that explicit backend dispatch cannot leak into std_fs injection.
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator std::filesystem::path() const { return "partial-backend"; }
};

struct partial_raw_ops {
  [[nodiscard]] static std::size_t
  hash_value(partial_raw_fs &backend,
             const std::filesystem::path &value) noexcept {
    ++backend.hash_calls;
    return value.native().size();
  }
};

struct hybrid_status {
  bool known{true};
  bool present{};
};

struct hybrid_path {
  bool present{};

  // Intentionally implicit: raw predicate dispatch must still require the
  // exact associated status type instead of accepting this conversion.
  // NOLINTNEXTLINE(hicpp-explicit-conversions)
  operator hybrid_status() const noexcept {
    return hybrid_status{.known = true, .present = present};
  }
};

struct hybrid_fs {
  int status_calls{};
  int status_known_calls{};
  int exists_calls{};
};

struct hybrid_ops {
  [[nodiscard]] static hybrid_status
  status(hybrid_fs &backend, const hybrid_path &value,
         std::error_code &error) noexcept {
    ++backend.status_calls;
    error.clear();
    return value;
  }

  [[nodiscard]] static bool
  status_known(hybrid_fs &backend, hybrid_status value) noexcept {
    ++backend.status_known_calls;
    return value.known;
  }

  [[nodiscard]] static bool
  exists(hybrid_fs &backend, hybrid_status value) noexcept {
    ++backend.exists_calls;
    return value.present;
  }
};

struct tracking_fs : expected_fs::std_fs_t {
  std::filesystem::path root{"tracked-root"};
  std::uintmax_t size{640};
  std::filesystem::file_time_type write_time;
  std::filesystem::space_info space_value{
      .capacity = 1000,
      .free = 600,
      .available = 400,
  };
  int call_count{};
  std::uintmax_t resized_to{};
  std::filesystem::copy_options last_copy_options{
      std::filesystem::copy_options::skip_existing};
  std::filesystem::copy_options last_copy_file_options{
      std::filesystem::copy_options::skip_existing};
  std::filesystem::perm_options last_permission_options{
      std::filesystem::perm_options::add};
  std::filesystem::directory_options last_directory_options{
      std::filesystem::directory_options::skip_permission_denied};
  std::filesystem::directory_options last_recursive_options{
      std::filesystem::directory_options::follow_directory_symlink};
};

struct tracking_ops : expected_fs::std_fs_ops {
  using base = expected_fs::std_fs_ops;
  using path = std::filesystem::path;
  using status_type = std::filesystem::file_status;
  using time_type = std::filesystem::file_time_type;

  [[nodiscard]] static path
  absolute(tracking_fs &backend, const path &value, std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return backend.root / value;
  }

  [[nodiscard]] static status_type
  status(tracking_fs &backend, const path &, std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return status_type{std::filesystem::file_type::regular};
  }

  [[nodiscard]] static status_type
  status(tracking_fs &backend, const tracking_entry &, std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return status_type{std::filesystem::file_type::directory};
  }

  [[nodiscard]] static status_type
  symlink_status(tracking_fs &backend, const path &, std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return status_type{std::filesystem::file_type::symlink};
  }

  [[nodiscard]] static status_type
  symlink_status(tracking_fs &backend, const tracking_entry &,
                 std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return status_type{std::filesystem::file_type::symlink};
  }

  [[nodiscard]] static bool
  equivalent(tracking_fs &backend, const path &left, const path &right,
             std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return left == right;
  }

  [[nodiscard]] static bool
  is_empty(tracking_fs &backend, const path &, std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return false;
  }

  [[nodiscard]] static bool
  is_empty(tracking_fs &backend, const tracking_entry &,
           std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return true;
  }

  [[nodiscard]] static std::uintmax_t
  file_size(tracking_fs &backend, const path &, std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return backend.size;
  }

  [[nodiscard]] static std::uintmax_t
  file_size(tracking_fs &backend, const tracking_entry &,
            std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return backend.size + 1;
  }

  using base::hard_link_count;

  [[nodiscard]] static std::uintmax_t
  hard_link_count(tracking_fs &backend, const tracking_entry &,
                  std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return 3;
  }

  [[nodiscard]] static time_type
  last_write_time(tracking_fs &backend, const path &, std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return backend.write_time;
  }

  [[nodiscard]] static time_type
  last_write_time(tracking_fs &backend, const tracking_entry &,
                  std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return backend.write_time;
  }

  static void last_write_time(tracking_fs &backend, const path &,
                              time_type value, std::error_code &error) {
    ++backend.call_count;
    backend.write_time = value;
    error.clear();
  }

  [[nodiscard]] static std::filesystem::space_info
  space(tracking_fs &backend, const path &, std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return backend.space_value;
  }

  static void
  copy(tracking_fs &backend, const path &, const path &,
       std::filesystem::copy_options options, std::error_code &error) {
    ++backend.call_count;
    backend.last_copy_options = options;
    error.clear();
  }

  [[nodiscard]] static bool
  copy_file(tracking_fs &backend, const path &, const path &,
            std::filesystem::copy_options options, std::error_code &error) {
    ++backend.call_count;
    backend.last_copy_file_options = options;
    error.clear();
    return true;
  }

  static void copy_symlink(tracking_fs &backend, const path &, const path &,
                           std::error_code &error) {
    ++backend.call_count;
    error.clear();
  }

  static void create_hard_link(tracking_fs &backend, const path &, const path &,
                               std::error_code &error) {
    ++backend.call_count;
    error.clear();
  }

  static void create_symlink(tracking_fs &backend, const path &, const path &,
                             std::error_code &error) {
    ++backend.call_count;
    error.clear();
  }

  static void create_directory_symlink(tracking_fs &backend, const path &,
                                       const path &, std::error_code &error) {
    ++backend.call_count;
    error.clear();
  }

  static void
  permissions(tracking_fs &backend, const path &, std::filesystem::perms,
              std::filesystem::perm_options options, std::error_code &error) {
    ++backend.call_count;
    backend.last_permission_options = options;
    error.clear();
  }

  static void resize_file(tracking_fs &backend, const path &,
                          std::uintmax_t value, std::error_code &error) {
    ++backend.call_count;
    backend.resized_to = value;
    error.clear();
  }

  [[nodiscard]] static tracking_entry
  make_directory_entry(tracking_fs &backend, const path &value,
                       std::error_code &error) {
    ++backend.call_count;
    error.clear();
    return tracking_entry{.value = value, .refresh_count = 0};
  }

  static void assign(tracking_fs &backend, tracking_entry &entry,
                     const path &value, std::error_code &error) {
    ++backend.call_count;
    entry.value = value;
    error.clear();
  }

  static void replace_filename(tracking_fs &backend, tracking_entry &entry,
                               const path &value, std::error_code &error) {
    ++backend.call_count;
    entry.value.replace_filename(value);
    error.clear();
  }

  static void
  refresh(tracking_fs &backend, tracking_entry &entry, std::error_code &error) {
    ++backend.call_count;
    ++entry.refresh_count;
    error.clear();
  }

  [[nodiscard]] static tracking_directory_iterator
  make_directory_iterator(tracking_fs &backend, const path &,
                          std::filesystem::directory_options options,
                          std::error_code &error) {
    ++backend.call_count;
    backend.last_directory_options = options;
    error.clear();
    return tracking_directory_iterator{1};
  }

  static void
  increment(tracking_fs &backend, tracking_directory_iterator &iterator,
            std::error_code &error) {
    ++backend.call_count;
    ++iterator.position;
    error.clear();
  }

  [[nodiscard]] static tracking_recursive_directory_iterator
  make_recursive_directory_iterator(tracking_fs &backend, const path &,
                                    std::filesystem::directory_options options,
                                    std::error_code &error) {
    ++backend.call_count;
    backend.last_recursive_options = options;
    error.clear();
    return tracking_recursive_directory_iterator{2};
  }

  static void increment(tracking_fs &backend,
                        tracking_recursive_directory_iterator &iterator,
                        std::error_code &error) {
    ++backend.call_count;
    ++iterator.position;
    error.clear();
  }

  static void
  pop(tracking_fs &backend, tracking_recursive_directory_iterator &iterator,
      std::error_code &error) {
    ++backend.call_count;
    --iterator.position;
    error.clear();
  }
};

struct priority_fs : expected_fs::std_fs_t {
  int tag_calls{};
  int policy_tag_calls{};
  int ops_calls{};
  int raw_tag_calls{};
  int raw_ops_calls{};
  int default_copy_tag_calls{};
  int copy_ops_calls{};
  std::filesystem::copy_options tagged_copy_options{
      std::filesystem::copy_options::skip_existing};

  friend expected_fs::expected<std::uintmax_t>
  tag_invoke(expected_fs::file_size_t, priority_fs &backend,
             const std::filesystem::path &) {
    ++backend.tag_calls;
    return std::uintmax_t{901};
  }

  friend std::size_t tag_invoke(expected_fs::hash_value_t, priority_fs &backend,
                                const std::filesystem::path &) noexcept {
    ++backend.raw_tag_calls;
    return 905;
  }

  template <class Fs, class Policy>
    requires std::same_as<std::remove_cvref_t<Fs>, priority_fs>
  friend auto tag_invoke(expected_fs::file_size_t,
                         expected_fs::result_adaptor<Fs, Policy> adaptor,
                         const std::filesystem::path &)
      -> Policy::template result<std::uintmax_t> {
    ++adaptor.fs.policy_tag_calls;
    return Policy::template success<std::uintmax_t>(902);
  }

  template <class Fs, class Policy>
    requires std::same_as<std::remove_cvref_t<Fs>, priority_fs>
  friend auto
  tag_invoke(expected_fs::copy_t,
             expected_fs::result_adaptor<Fs, Policy> adaptor,
             const std::filesystem::path &, const std::filesystem::path &,
             std::filesystem::copy_options options)
      -> Policy::template result<void> {
    ++adaptor.fs.default_copy_tag_calls;
    adaptor.fs.tagged_copy_options = options;
    return Policy::success();
  }
};

struct priority_ops : expected_fs::std_fs_ops {
  [[nodiscard]] static std::uintmax_t
  file_size(priority_fs &backend, const std::filesystem::path &,
            std::error_code &error) {
    ++backend.ops_calls;
    error.clear();
    return 903;
  }

  static void copy(priority_fs &backend, const std::filesystem::path &,
                   const std::filesystem::path &, std::filesystem::copy_options,
                   std::error_code &error) {
    ++backend.copy_ops_calls;
    error.clear();
  }

  [[nodiscard]] static std::size_t
  hash_value(priority_fs &backend, const std::filesystem::path &) noexcept {
    ++backend.raw_ops_calls;
    return 906;
  }
};

struct default_injected_path {
  int *tag_calls;
};

[[nodiscard]] inline expected_fs::expected<std::uintmax_t>
tag_invoke(expected_fs::file_size_t, const expected_fs::std_fs_t &,
           default_injected_path value) {
  ++*value.tag_calls;
  return std::uintmax_t{904};
}
} // namespace test_backend
} // namespace

namespace expected_fs {
template <>
struct fs_traits<test_backend::std_like_fs> {
  using ops = test_backend::std_like_ops;
  using path = test_backend::std_like_path;
};

template <>
struct fs_traits<test_backend::convertible_fs> {
  using ops = test_backend::convertible_ops;
  using path = test_backend::std_like_path;
};

template <>
struct fs_traits<test_backend::tracking_fs> : fs_traits<std_fs_t> {
  using ops = test_backend::tracking_ops;
  using directory_entry = test_backend::tracking_entry;
  using directory_iterator = test_backend::tracking_directory_iterator;
  using recursive_directory_iterator =
      test_backend::tracking_recursive_directory_iterator;
};

template <>
struct fs_traits<test_backend::raw_fs> : fs_traits<std_fs_t> {
  using ops = test_backend::raw_ops;
  using path = test_backend::raw_path;
  using file_status = test_backend::raw_status;
  using directory_iterator = test_backend::raw_directory_iterator;
  using recursive_directory_iterator =
      test_backend::raw_recursive_directory_iterator;
};

template <>
struct fs_traits<test_backend::partial_raw_fs> : fs_traits<std_fs_t> {
  using ops = test_backend::partial_raw_ops;
};

template <>
struct fs_traits<test_backend::hybrid_fs> {
  using ops = test_backend::hybrid_ops;
  using path = test_backend::hybrid_path;
  using file_status = test_backend::hybrid_status;
};

template <>
struct fs_traits<test_backend::priority_fs> : fs_traits<std_fs_t> {
  using ops = test_backend::priority_ops;
};
} // namespace expected_fs

namespace {
namespace test_result {
struct domain {};

struct stateful_domain {
  int state{};
};

struct non_default_domain {
  non_default_domain() = delete;
};

template <class T, class E>
struct result {
  std::expected<T, E> storage;

  [[nodiscard]] bool ok() const { return storage.has_value(); }

  [[nodiscard]] T &get() & { return *storage; }
  [[nodiscard]] const T &get() const & { return *storage; }
  [[nodiscard]] T &&get() && { return std::move(*storage); }

  [[nodiscard]] E &error_value() & { return storage.error(); }
  [[nodiscard]] const E &error_value() const & { return storage.error(); }
  [[nodiscard]] E &&error_value() && { return std::move(storage).error(); }
};

template <class E>
struct result<void, E> {
  std::expected<void, E> storage;

  [[nodiscard]] bool ok() const { return storage.has_value(); }

  [[nodiscard]] E &error_value() & { return storage.error(); }
  [[nodiscard]] const E &error_value() const & { return storage.error(); }
  [[nodiscard]] E &&error_value() && { return std::move(storage).error(); }
};
} // namespace test_result
} // namespace

namespace expected_fs {
template <>
struct result_domain_traits<test_result::domain> {
  template <class T, class E>
  using result = test_result::result<T, E>;
};

template <>
struct result_domain_traits<test_result::stateful_domain> {
  template <class T, class E>
  using result = test_result::result<T, E>;
};

template <>
struct result_domain_traits<test_result::non_default_domain> {
  template <class T, class E>
  using result = test_result::result<T, E>;
};

template <class T, class E, class Value>
  requires(!std::is_void_v<T>)
[[nodiscard]] static auto
tag_invoke(result_success_t, test_result::domain, std::type_identity<T>,
           std::type_identity<E>, Value &&value)
    -> result_t<test_result::domain, T, E> {
  return result_t<test_result::domain, T, E>{
      std::expected<T, E>{std::forward<Value>(value)},
  };
}

template <class E>
[[nodiscard]] static auto
tag_invoke(result_success_t, test_result::domain, std::type_identity<void>,
           std::type_identity<E>) -> result_t<test_result::domain, void, E> {
  return result_t<test_result::domain, void, E>{std::expected<void, E>{}};
}

template <class T, class E, class Err>
[[nodiscard]] static auto
tag_invoke(result_failure_t, test_result::domain, std::type_identity<T>,
           std::type_identity<E>, Err &&error)
    -> result_t<test_result::domain, T, E> {
  return result_t<test_result::domain, T, E>{
      std::expected<T, E>{std::unexpected(E{std::forward<Err>(error)})},
  };
}
} // namespace expected_fs

namespace {
namespace fs = std::filesystem;

class temporary_directory {
public:
  temporary_directory() {
    const auto temp = fs::temp_directory_path();
    const auto seed =
        std::chrono::steady_clock::now().time_since_epoch().count();

    for (int attempt = 0; attempt < 100; ++attempt) {
      const auto candidate = temp
                           / ("expected_fs_test_"
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

class current_path_guard {
public:
  explicit current_path_guard(fs::path original_path)
      : original_path_(std::move(original_path)) {}

  current_path_guard(const current_path_guard &) = delete;
  current_path_guard &operator=(const current_path_guard &) = delete;
  current_path_guard(current_path_guard &&) = delete;
  current_path_guard &operator=(current_path_guard &&) = delete;

  ~current_path_guard() {
    std::error_code error;
    fs::current_path(original_path_, error);
  }

private:
  fs::path original_path_;
};

void write_text(const fs::path &path, const std::string &contents) {
  std::ofstream stream(path, std::ios::binary);
  if (!stream) {
    throw std::runtime_error("failed to open test file for writing");
  }

  stream << contents;
}

struct fs_error {
  int value{};
};

struct mapped_error {
  int value{};

  explicit mapped_error(std::error_code error) : value(error.value()) {}
  explicit mapped_error(fs_error error) : value(error.value) {}
};

struct mapped_size {
  std::uintmax_t value{};

  explicit mapped_size(std::uintmax_t size) : value(size) {}
};

struct unit_value {
  bool value{true};
};

struct custom_result_policy {
  template <class T>
  using result = std::expected<T, mapped_error>;

  template <class T>
  [[nodiscard]] static result<T> success(T value) {
    return result<T>{std::move(value)};
  }

  [[nodiscard]] static result<void> success() { return {}; }

  template <class T, class Err>
  [[nodiscard]] static result<T> failure(Err &&error) {
    return std::unexpected(mapped_error{std::forward<Err>(error)});
  }
};

template <class T>
struct rigid_result {
  explicit rigid_result(std::expected<T, mapped_error> value)
      : storage(std::move(value)) {}

  template <class U>
  rigid_result(const rigid_result<U> &) = delete;

  template <class U>
  rigid_result(rigid_result<U> &&) = delete;

  [[nodiscard]] bool has_value() const { return storage.has_value(); }
  [[nodiscard]] T &operator*() & { return *storage; }
  [[nodiscard]] const T &operator*() const & { return *storage; }
  [[nodiscard]] mapped_error &error() & { return storage.error(); }
  [[nodiscard]] const mapped_error &error() const & { return storage.error(); }

  std::expected<T, mapped_error> storage;
};

struct rigid_result_policy {
  template <class T>
  using result = rigid_result<T>;

  template <class T>
  [[nodiscard]] static result<T> success(T value) {
    return result<T>{std::expected<T, mapped_error>{std::move(value)}};
  }

  template <class T, class Err>
  [[nodiscard]] static result<T> failure(Err &&error) {
    return result<T>{std::expected<T, mapped_error>{
        std::unexpected(mapped_error{std::forward<Err>(error)})}};
  }
};

struct custom_path {
  bool missing{};
};

struct custom_fs {
  std::uintmax_t size{42};
  int error_value{77};

  friend std::expected<std::uintmax_t, fs_error>
  tag_invoke(expected_fs::file_size_t, custom_fs fs, custom_path value) {
    if (value.missing) {
      return std::unexpected(fs_error{fs.error_value});
    }

    return fs.size;
  }

  template <class Fs, class Policy>
    requires std::same_as<std::remove_cvref_t<Fs>, custom_fs>
  friend auto
  tag_invoke(expected_fs::file_size_t,
             expected_fs::result_adaptor<Fs, Policy> adaptor, custom_path value)
      -> Policy::template result<std::uintmax_t> {
    if (value.missing) {
      return Policy::template failure<std::uintmax_t>(
          fs_error{adaptor.fs.error_value});
    }

    return Policy::template success<std::uintmax_t>(adaptor.fs.size);
  }
};

struct fallback_only_fs {
  friend std::expected<std::uintmax_t, fs_error>
  tag_invoke(expected_fs::file_size_t, fallback_only_fs, custom_path value) {
    if (value.missing) {
      return std::unexpected(fs_error{77});
    }

    return std::uintmax_t{42};
  }
};

struct alias_backend {
  using fs_type = void;
  using policy_type = void;

  std::uintmax_t size{640};

  template <class Fs, class Policy>
    requires std::same_as<std::remove_cvref_t<Fs>, alias_backend>
  friend auto
  tag_invoke(expected_fs::file_size_t,
             expected_fs::result_adaptor<Fs, Policy> adaptor, custom_path value)
      -> Policy::template result<std::uintmax_t> {
    if (value.missing) {
      return Policy::template failure<std::uintmax_t>(
          std::make_error_code(std::errc::no_such_file_or_directory));
    }

    return Policy::template success<std::uintmax_t>(adaptor.fs.size);
  }
};

template <class Fs>
concept has_policy_file_size = requires {
  expected_fs::file_size(
      expected_fs::with_policy<custom_result_policy>(std::declval<Fs>()),
      custom_path{});
};

template <class Fs>
concept has_error_file_size = requires {
  expected_fs::file_size(
      expected_fs::with_error<mapped_error>(std::declval<Fs>()), custom_path{});
};

template <class Fs>
concept has_std_like_file_size = requires(Fs &backend) {
  expected_fs::file_size(backend, test_backend::std_like_path{});
};

template <class Fs>
concept has_std_like_absolute = requires(Fs &backend) {
  expected_fs::absolute(backend, test_backend::std_like_path{});
};

template <class Fs>
concept has_std_like_current_path =
    requires(Fs &backend) { expected_fs::current_path(backend); };

template <class Fs>
concept has_std_like_relative = requires(Fs &backend) {
  expected_fs::relative(backend, test_backend::std_like_path{});
};

template <class Fs>
concept has_std_like_create_directory = requires(Fs &backend) {
  expected_fs::create_directory(backend, test_backend::std_like_path{});
};

template <class Operation, class Dispatch, class... Args>
concept raw_dispatch_invocable = requires {
  std::declval<Operation>()(std::declval<Dispatch>(), std::declval<Args>()...);
};

template <class Dispatch>
inline constexpr bool rejects_raw_wrappers_except_hash_v =
    !raw_dispatch_invocable<expected_fs::u8path_t, Dispatch, std::string>
    && !raw_dispatch_invocable<expected_fs::u8path_t, Dispatch,
                               std::string::const_iterator,
                               std::string::const_iterator>
    && !raw_dispatch_invocable<expected_fs::swap_t, Dispatch, fs::path &,
                               fs::path &>
    && !raw_dispatch_invocable<expected_fs::status_known_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::exists_t, Dispatch, fs::file_status>
    && !raw_dispatch_invocable<expected_fs::is_block_file_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::is_character_file_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::is_directory_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::is_fifo_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::is_other_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::is_regular_file_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::is_socket_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::is_symlink_t, Dispatch,
                               fs::file_status>
    && !raw_dispatch_invocable<expected_fs::begin_t, Dispatch,
                               fs::directory_iterator>
    && !raw_dispatch_invocable<expected_fs::end_t, Dispatch,
                               fs::directory_iterator>
    && !raw_dispatch_invocable<expected_fs::begin_t, Dispatch,
                               fs::recursive_directory_iterator>
    && !raw_dispatch_invocable<expected_fs::end_t, Dispatch,
                               fs::recursive_directory_iterator>
    && !raw_dispatch_invocable<expected_fs::options_t, Dispatch,
                               const fs::recursive_directory_iterator &>
    && !raw_dispatch_invocable<expected_fs::depth_t, Dispatch,
                               const fs::recursive_directory_iterator &>
    && !raw_dispatch_invocable<expected_fs::recursion_pending_t, Dispatch,
                               const fs::recursive_directory_iterator &>
    && !raw_dispatch_invocable<expected_fs::disable_recursion_pending_t,
                               Dispatch, fs::recursive_directory_iterator &>;

template <class Dispatch>
inline constexpr bool rejects_all_raw_wrappers_v =
    rejects_raw_wrappers_except_hash_v<Dispatch>
    && !raw_dispatch_invocable<expected_fs::hash_value_t, Dispatch,
                               const fs::path &>;

template <class Domain>
concept supports_result_domain =
    requires(Domain domain) { expected_fs::with_result(domain); };
} // namespace

TEST_CASE("expected_fs exposes std::expected compatible types", "[api]") {
  using std_result_adaptor =
      expected_fs::result_adaptor<expected_fs::std_fs_t,
                                  expected_fs::std_result_policy<>>;
  using copied_backend_adaptor =
      decltype(expected_fs::with_policy<custom_result_policy>(
          std::declval<custom_fs &>()));
  using borrowed_backend_adaptor =
      decltype(expected_fs::with_policy<custom_result_policy>(
          std::ref(std::declval<custom_fs &>())));
  using existing_adaptor = decltype(expected_fs::with_error<mapped_error>(
      std::declval<test_backend::std_like_fs>()));
  using replaced_policy_adaptor =
      decltype(expected_fs::with_policy<custom_result_policy>(
          std::declval<existing_adaptor &>()));
  using replaced_domain_adaptor = decltype(expected_fs::with_result(
      test_result::domain{}, std::declval<existing_adaptor &>()));
  using test_domain_policy =
      expected_fs::result_policy<test_result::domain, std::error_code>;

  static_assert(std::is_same_v<expected_fs::fs_path_t<expected_fs::std_fs_t>,
                               std::filesystem::path>);
  static_assert(
      std::is_same_v<expected_fs::fs_path_t<const expected_fs::std_fs_t &>,
                     std::filesystem::path>);
  static_assert(
      std::is_same_v<expected_fs::fs_directory_entry_t<std_result_adaptor>,
                     std::filesystem::directory_entry>);
  static_assert(
      std::is_same_v<expected_fs::fs_directory_iterator_t<std_result_adaptor>,
                     std::filesystem::directory_iterator>);
  static_assert(
      std::is_same_v<
          expected_fs::fs_recursive_directory_iterator_t<std_result_adaptor>,
          std::filesystem::recursive_directory_iterator>);
  static_assert(
      std::is_same_v<expected_fs::fs_path_t<test_backend::std_like_fs>,
                     test_backend::std_like_path>);
  static_assert(has_std_like_file_size<test_backend::std_like_fs>);
  static_assert(!has_std_like_absolute<test_backend::std_like_fs>);
  static_assert(!has_std_like_current_path<test_backend::std_like_fs>);
  static_assert(!has_std_like_relative<test_backend::std_like_fs>);
  static_assert(!has_std_like_create_directory<test_backend::std_like_fs>);
  static_assert(std::is_same_v<
                expected_fs::fs_directory_entry_t<test_backend::tracking_fs>,
                test_backend::tracking_entry>);
  static_assert(std::is_same_v<
                expected_fs::fs_directory_iterator_t<test_backend::tracking_fs>,
                test_backend::tracking_directory_iterator>);
  static_assert(
      std::is_same_v<expected_fs::fs_recursive_directory_iterator_t<
                         test_backend::tracking_fs>,
                     test_backend::tracking_recursive_directory_iterator>);
  static_assert(
      std::is_same_v<expected_fs::result_t<expected_fs::std_result_domain, int,
                                           std::error_code>,
                     std::expected<int, std::error_code>>);
  static_assert(
      std::is_same_v<
          expected_fs::result_t<test_result::domain, int, std::error_code>,
          test_result::result<int, std::error_code>>);
  static_assert(
      std::convertible_to<test_backend::convertible_size, std::uintmax_t>);
  static_assert(
      !std::is_convertible_v<rigid_result<test_backend::convertible_size>,
                             rigid_result<std::uintmax_t>>);
  static_assert(std::is_same_v<copied_backend_adaptor::fs_type, custom_fs>);
  static_assert(std::is_same_v<borrowed_backend_adaptor::fs_type, custom_fs &>);
  static_assert(std::is_same_v<replaced_policy_adaptor::fs_type,
                               test_backend::std_like_fs>);
  static_assert(std::is_same_v<replaced_policy_adaptor::policy_type,
                               custom_result_policy>);
  static_assert(std::is_same_v<replaced_domain_adaptor::fs_type,
                               test_backend::std_like_fs>);
  static_assert(
      std::is_same_v<replaced_domain_adaptor::policy_type, test_domain_policy>);
  static_assert(supports_result_domain<test_result::domain>);
  static_assert(!supports_result_domain<test_result::stateful_domain>);
  static_assert(!supports_result_domain<test_result::non_default_domain>);
  static_assert(std::is_same_v<expected_fs::path, std::filesystem::path>);
  static_assert(std::is_same_v<expected_fs::directory_entry,
                               std::filesystem::directory_entry>);
  static_assert(std::is_same_v<expected_fs::directory_iterator,
                               std::filesystem::directory_iterator>);
  static_assert(std::is_same_v<expected_fs::recursive_directory_iterator,
                               std::filesystem::recursive_directory_iterator>);
  static_assert(
      std::is_same_v<expected_fs::file_status, std::filesystem::file_status>);
  static_assert(
      std::is_same_v<expected_fs::space_info, std::filesystem::space_info>);
  static_assert(std::is_same_v<expected_fs::expected<int>,
                               std::expected<int, std::error_code>>);
  static_assert(std::is_same_v<expected_fs::expected<int, std::errc>,
                               std::expected<int, std::errc>>);
  static_assert(
      noexcept(expected_fs::exists(std::declval<expected_fs::file_status>())));
  static_assert(noexcept(
      expected_fs::is_block_file(std::declval<expected_fs::file_status>())));
  static_assert(noexcept(expected_fs::is_character_file(
      std::declval<expected_fs::file_status>())));
  static_assert(noexcept(
      expected_fs::is_directory(std::declval<expected_fs::file_status>())));
  static_assert(
      noexcept(expected_fs::is_fifo(std::declval<expected_fs::file_status>())));
  static_assert(noexcept(
      expected_fs::is_other(std::declval<expected_fs::file_status>())));
  static_assert(noexcept(
      expected_fs::is_regular_file(std::declval<expected_fs::file_status>())));
  static_assert(noexcept(
      expected_fs::is_socket(std::declval<expected_fs::file_status>())));
  static_assert(noexcept(
      expected_fs::is_symlink(std::declval<expected_fs::file_status>())));
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   std::declval<const fs::path &>())),
                               std::expected<std::uintmax_t, std::error_code>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   expected_fs::with_error<mapped_error>,
                                   std::declval<const fs::path &>())),
                               std::expected<std::uintmax_t, mapped_error>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   expected_fs::with_value<mapped_size>,
                                   std::declval<const fs::path &>())),
                               std::expected<mapped_size, std::error_code>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   expected_fs::with_value<void>,
                                   std::declval<const fs::path &>())),
                               std::expected<void, std::error_code>>);
  static_assert(std::is_same_v<decltype(expected_fs::exists(
                                   expected_fs::with_value<void>,
                                   std::declval<const fs::path &>())),
                               std::expected<void, std::error_code>>);
  static_assert(std::is_same_v<decltype(expected_fs::is_symlink(
                                   expected_fs::with_value<void>,
                                   std::declval<const fs::path &>())),
                               std::expected<void, std::error_code>>);
  static_assert(std::is_same_v<decltype(expected_fs::current_path(
                                   expected_fs::with_value<unit_value>,
                                   std::declval<const fs::path &>())),
                               std::expected<unit_value, std::error_code>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   expected_fs::with_error<mapped_error>(
                                       expected_fs::with_value<mapped_size>),
                                   std::declval<const fs::path &>())),
                               std::expected<mapped_size, mapped_error>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   expected_fs::with_value<mapped_size>(
                                       expected_fs::with_error<mapped_error>),
                                   std::declval<const fs::path &>())),
                               std::expected<mapped_size, mapped_error>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         expected_fs::with_policy<custom_result_policy>,
                         std::declval<const fs::path &>())),
                     custom_result_policy::result<std::uintmax_t>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         expected_fs::with_result(expected_fs::std_result),
                         std::declval<const fs::path &>())),
                     std::expected<std::uintmax_t, std::error_code>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         expected_fs::with_result(test_result::domain{}),
                         std::declval<const fs::path &>())),
                     test_result::result<std::uintmax_t, std::error_code>>);
  static_assert(std::is_same_v<
                decltype(expected_fs::file_size(
                    expected_fs::with_value<mapped_size>(test_result::domain{}),
                    std::declval<const fs::path &>())),
                test_result::result<mapped_size, std::error_code>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   expected_fs::with_error<mapped_error>(
                                       expected_fs::with_value<mapped_size>(
                                           test_result::domain{})),
                                   std::declval<const fs::path &>())),
                               test_result::result<mapped_size, mapped_error>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::exists(
                         expected_fs::with_result(test_result::domain{}),
                         std::declval<const fs::path &>())),
                     test_result::result<bool, std::error_code>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   std::declval<custom_fs>(),
                                   std::declval<custom_path>())),
                               std::expected<std::uintmax_t, fs_error>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   expected_fs::with_error<mapped_error>(
                                       std::declval<custom_fs>()),
                                   std::declval<custom_path>())),
                               std::expected<std::uintmax_t, mapped_error>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   expected_fs::with_error<mapped_error>(
                                       expected_fs::with_value<mapped_size>(
                                           std::declval<custom_fs>())),
                                   std::declval<custom_path>())),
                               std::expected<mapped_size, mapped_error>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(
                                   std::declval<fallback_only_fs>(),
                                   std::declval<custom_path>())),
                               std::expected<std::uintmax_t, fs_error>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         std::declval<test_backend::std_like_fs>(),
                         std::declval<test_backend::std_like_path>())),
                     expected_fs::expected<std::uintmax_t>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         expected_fs::with_error<mapped_error>(
                             std::declval<test_backend::std_like_fs>()),
                         std::declval<test_backend::std_like_path>())),
                     std::expected<std::uintmax_t, mapped_error>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         expected_fs::with_error<mapped_error>(
                             test_result::domain{}),
                         std::declval<const fs::path &>())),
                     test_result::result<std::uintmax_t, mapped_error>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         expected_fs::with_policy<rigid_result_policy>(
                             test_backend::convertible_fs{}),
                         test_backend::std_like_path{})),
                     rigid_result<std::uintmax_t>>);
  static_assert(std::is_same_v<decltype(expected_fs::file_size(alias_backend{},
                                                               custom_path{})),
                               expected_fs::expected<std::uintmax_t>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         expected_fs::with_error<mapped_error>(alias_backend{}),
                         custom_path{})),
                     std::expected<std::uintmax_t, mapped_error>>);
  static_assert(!has_policy_file_size<fallback_only_fs>);
  static_assert(!has_error_file_size<fallback_only_fs>);
#if EXPECTED_FS_HAS_FORMAT_PATH
  static_assert(expected_fs::has_format_path);
#else
  static_assert(!expected_fs::has_format_path);
#endif
  static_assert(std::is_same_v<decltype(expected_fs::exists(
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<bool>>);
  static_assert(std::is_same_v<decltype(expected_fs::exists(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(
      std::is_same_v<decltype(expected_fs::exists(
                         std::declval<const fs::directory_entry &>())),
                     expected_fs::expected<bool>>);
  static_assert(std::is_same_v<decltype(expected_fs::current_path(
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::status_known(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_block_file(
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<bool>>);
  static_assert(std::is_same_v<decltype(expected_fs::is_block_file(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(
      std::is_same_v<decltype(expected_fs::is_character_file(
                         std::declval<const fs::directory_entry &>())),
                     expected_fs::expected<bool>>);
  static_assert(std::is_same_v<decltype(expected_fs::is_fifo(
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<bool>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::is_other(
                         std::declval<const fs::directory_entry &>())),
                     expected_fs::expected<bool>>);
  static_assert(std::is_same_v<decltype(expected_fs::is_socket(
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<bool>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::status(
                         std::declval<const fs::directory_entry &>())),
                     expected_fs::expected<fs::file_status>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::symlink_status(
                         std::declval<const fs::directory_entry &>())),
                     expected_fs::expected<fs::file_status>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::file_size(
                         std::declval<const fs::directory_entry &>())),
                     expected_fs::expected<std::uintmax_t>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::last_write_time(
                         std::declval<const fs::directory_entry &>())),
                     expected_fs::expected<fs::file_time_type>>);
  static_assert(std::is_same_v<decltype(expected_fs::copy_symlink(
                                   std::declval<const fs::path &>(),
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::create_directory_symlink(
                                   std::declval<const fs::path &>(),
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::create_symlink(
                                   std::declval<const fs::path &>(),
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::create_hard_link(
                                   std::declval<const fs::path &>(),
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::make_directory_entry(
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<fs::directory_entry>>);
  static_assert(std::is_same_v<decltype(expected_fs::assign(
                                   std::declval<fs::directory_entry &>(),
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::replace_filename(
                                   std::declval<fs::directory_entry &>(),
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::refresh(
                                   std::declval<fs::directory_entry &>())),
                               expected_fs::expected<void>>);
  static_assert(std::is_same_v<decltype(expected_fs::make_directory_iterator(
                                   std::declval<const fs::path &>())),
                               expected_fs::expected<fs::directory_iterator>>);
  static_assert(std::is_same_v<decltype(expected_fs::increment(
                                   std::declval<fs::directory_iterator &>())),
                               expected_fs::expected<void>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::make_recursive_directory_iterator(
                         std::declval<const fs::path &>())),
                     expected_fs::expected<fs::recursive_directory_iterator>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::increment(
                         std::declval<fs::recursive_directory_iterator &>())),
                     expected_fs::expected<void>>);
  static_assert(
      std::is_same_v<decltype(expected_fs::pop(
                         std::declval<fs::recursive_directory_iterator &>())),
                     expected_fs::expected<void>>);
  static_assert(std::is_same_v<
                decltype(expected_fs::depth(
                    std::declval<const fs::recursive_directory_iterator &>())),
                int>);
  static_assert(std::is_same_v<
                decltype(expected_fs::recursion_pending(
                    std::declval<const fs::recursive_directory_iterator &>())),
                bool>);

  REQUIRE(true);
}

TEST_CASE("raw named wrappers expose value semantics without result adaptors",
          "[api][raw]") {
  using string_iterator = std::string::const_iterator;
  using error_closure = decltype(expected_fs::with_error<mapped_error>);
  using value_closure = decltype(expected_fs::with_value<mapped_size>);
  using policy_closure =
      decltype(expected_fs::with_policy<custom_result_policy>);
  using domain_closure =
      decltype(expected_fs::with_result(test_result::domain{}));
  using error_adaptor =
      decltype(expected_fs::with_error<mapped_error>(expected_fs::std_fs));

  static_assert(std::is_same_v<decltype(expected_fs::u8path(
                                   std::declval<const std::string &>())),
                               fs::path>);
  static_assert(std::is_same_v<decltype(expected_fs::u8path(
                                   std::declval<string_iterator>(),
                                   std::declval<string_iterator>())),
                               fs::path>);
  static_assert(
      !noexcept(expected_fs::u8path(std::declval<const std::string &>())));
  static_assert(!noexcept(expected_fs::u8path(
      std::declval<string_iterator>(), std::declval<string_iterator>())));

  static_assert(
      std::is_same_v<decltype(expected_fs::swap(std::declval<fs::path &>(),
                                                std::declval<fs::path &>())),
                     void>);
  static_assert(noexcept(expected_fs::swap(std::declval<fs::path &>(),
                                           std::declval<fs::path &>())));
  static_assert(std::is_same_v<decltype(expected_fs::hash_value(
                                   std::declval<const fs::path &>())),
                               std::size_t>);
  static_assert(
      noexcept(expected_fs::hash_value(std::declval<const fs::path &>())));
  static_assert(std::is_same_v<decltype(expected_fs::status_known(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(
      noexcept(expected_fs::status_known(std::declval<fs::file_status>())));

  static_assert(std::is_same_v<decltype(expected_fs::begin(
                                   std::declval<fs::directory_iterator>())),
                               fs::directory_iterator>);
  static_assert(std::is_same_v<decltype(expected_fs::end(
                                   std::declval<fs::directory_iterator>())),
                               fs::directory_iterator>);
  static_assert(
      noexcept(expected_fs::begin(std::declval<fs::directory_iterator>())));
  static_assert(
      noexcept(expected_fs::end(std::declval<fs::directory_iterator>())));
  static_assert(
      std::is_same_v<decltype(expected_fs::begin(
                         std::declval<fs::recursive_directory_iterator>())),
                     fs::recursive_directory_iterator>);
  static_assert(
      std::is_same_v<decltype(expected_fs::end(
                         std::declval<fs::recursive_directory_iterator>())),
                     fs::recursive_directory_iterator>);
  static_assert(noexcept(
      expected_fs::begin(std::declval<fs::recursive_directory_iterator>())));
  static_assert(noexcept(
      expected_fs::end(std::declval<fs::recursive_directory_iterator>())));
  static_assert(std::is_same_v<
                decltype(expected_fs::options(
                    std::declval<const fs::recursive_directory_iterator &>())),
                fs::directory_options>);
  static_assert(noexcept(expected_fs::options(
      std::declval<const fs::recursive_directory_iterator &>())));
  static_assert(std::is_same_v<
                decltype(expected_fs::depth(
                    std::declval<const fs::recursive_directory_iterator &>())),
                int>);
  static_assert(noexcept(expected_fs::depth(
      std::declval<const fs::recursive_directory_iterator &>())));
  static_assert(std::is_same_v<
                decltype(expected_fs::recursion_pending(
                    std::declval<const fs::recursive_directory_iterator &>())),
                bool>);
  static_assert(noexcept(expected_fs::recursion_pending(
      std::declval<const fs::recursive_directory_iterator &>())));
  static_assert(
      std::is_same_v<decltype(expected_fs::disable_recursion_pending(
                         std::declval<fs::recursive_directory_iterator &>())),
                     void>);
  static_assert(noexcept(expected_fs::disable_recursion_pending(
      std::declval<fs::recursive_directory_iterator &>())));

  static_assert(std::is_same_v<decltype(expected_fs::exists(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_block_file(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_character_file(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_directory(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_fifo(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_other(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_regular_file(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_socket(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(std::is_same_v<decltype(expected_fs::is_symlink(
                                   std::declval<fs::file_status>())),
                               bool>);
  static_assert(noexcept(expected_fs::exists(std::declval<fs::file_status>())));
  static_assert(
      noexcept(expected_fs::is_block_file(std::declval<fs::file_status>())));
  static_assert(noexcept(
      expected_fs::is_character_file(std::declval<fs::file_status>())));
  static_assert(
      noexcept(expected_fs::is_directory(std::declval<fs::file_status>())));
  static_assert(
      noexcept(expected_fs::is_fifo(std::declval<fs::file_status>())));
  static_assert(
      noexcept(expected_fs::is_other(std::declval<fs::file_status>())));
  static_assert(
      noexcept(expected_fs::is_regular_file(std::declval<fs::file_status>())));
  static_assert(
      noexcept(expected_fs::is_socket(std::declval<fs::file_status>())));
  static_assert(
      noexcept(expected_fs::is_symlink(std::declval<fs::file_status>())));

  static_assert(
      raw_dispatch_invocable<expected_fs::hash_value_t,
                             test_backend::partial_raw_fs &, const fs::path &>);
  static_assert(
      rejects_raw_wrappers_except_hash_v<test_backend::partial_raw_fs &>);
  static_assert(rejects_all_raw_wrappers_v<error_closure>);
  static_assert(rejects_all_raw_wrappers_v<value_closure>);
  static_assert(rejects_all_raw_wrappers_v<policy_closure>);
  static_assert(rejects_all_raw_wrappers_v<domain_closure>);
  static_assert(rejects_all_raw_wrappers_v<error_adaptor>);

  test_backend::partial_raw_fs partial_backend;
  const fs::path value{"hash"};
  REQUIRE(expected_fs::hash_value(partial_backend, value)
          == value.native().size());
  REQUIRE(partial_backend.hash_calls == 1);
}

TEST_CASE("raw status predicates do not capture convertible associated paths",
          "[api][raw][customization]") {
  using hybrid_fs = test_backend::hybrid_fs;
  using hybrid_path = test_backend::hybrid_path;
  using hybrid_status = test_backend::hybrid_status;

  static_assert(std::convertible_to<hybrid_path, hybrid_status>);
  static_assert(std::is_same_v<decltype(expected_fs::exists(
                                   std::declval<hybrid_fs &>(),
                                   std::declval<const hybrid_path &>())),
                               expected_fs::expected<bool>>);
  static_assert(std::is_same_v<decltype(expected_fs::exists(
                                   std::declval<hybrid_fs &>(),
                                   std::declval<hybrid_status>())),
                               bool>);

  hybrid_fs backend;
  const auto fallible = expected_fs::exists(backend, hybrid_path{true});
  REQUIRE(fallible.has_value());
  REQUIRE(*fallible);
  REQUIRE(backend.status_calls == 1);
  REQUIRE(backend.status_known_calls == 1);
  REQUIRE(backend.exists_calls == 1);

  REQUIRE_FALSE(expected_fs::exists(
      backend, hybrid_status{.known = true, .present = false}));
  REQUIRE(backend.status_calls == 1);
  REQUIRE(backend.status_known_calls == 1);
  REQUIRE(backend.exists_calls == 2);
}

TEST_CASE("expected_fs exposes C++26 path formatting when available", "[api]") {
#if EXPECTED_FS_HAS_FORMAT_PATH
  const auto formatted =
      std::format("{}", expected_fs::path{"expected_fs.txt"});
  REQUIRE_FALSE(formatted.empty());
  REQUIRE(formatted.contains("expected_fs"));
#else
  REQUIRE_FALSE(expected_fs::has_format_path);
#endif
}

TEST_CASE(
    "expected_fs supports result domains, policy escape hatch, and custom "
    "filesystems",
    "[api]") {
  const temporary_directory directory;
  const auto source = directory.path() / "source.txt";
  write_text(source, "hello");

  const auto mapped_size_result =
      expected_fs::file_size(expected_fs::with_error<mapped_error>, source);
  REQUIRE(mapped_size_result.has_value());
  REQUIRE(*mapped_size_result == std::uintmax_t{5});

  const auto value_size =
      expected_fs::file_size(expected_fs::with_value<mapped_size>, source);
  REQUIRE(value_size.has_value());
  REQUIRE(value_size->value == std::uintmax_t{5});

  const auto discarded_size =
      expected_fs::file_size(expected_fs::with_value<void>, source);
  REQUIRE(discarded_size.has_value());

  const auto discarded_exists =
      expected_fs::exists(expected_fs::with_value<void>, source);
  REQUIRE(discarded_exists.has_value());

  const auto discarded_missing_exists = expected_fs::exists(
      expected_fs::with_value<void>, directory.path() / "missing.txt");
  REQUIRE(discarded_missing_exists.has_value());

  const auto discarded_symlink =
      expected_fs::is_symlink(expected_fs::with_value<void>, source);
  REQUIRE(discarded_symlink.has_value());

  const auto mapped_missing = expected_fs::file_size(
      expected_fs::with_error<mapped_error>, directory.path() / "missing.txt");
  REQUIRE_FALSE(mapped_missing.has_value());
  REQUIRE(mapped_missing.error().value != 0);

  const auto mapped_value_missing =
      expected_fs::file_size(expected_fs::with_error<mapped_error>(
                                 expected_fs::with_value<mapped_size>),
                             directory.path() / "missing.txt");
  REQUIRE_FALSE(mapped_value_missing.has_value());
  REQUIRE(mapped_value_missing.error().value != 0);

  const auto policy_size = expected_fs::file_size(
      expected_fs::with_policy<custom_result_policy>, source);
  REQUIRE(policy_size.has_value());
  REQUIRE(*policy_size == std::uintmax_t{5});

  const auto domain_size = expected_fs::file_size(
      expected_fs::with_result(test_result::domain{}), source);
  REQUIRE(domain_size.ok());
  REQUIRE(domain_size.get() == std::uintmax_t{5});

  const auto domain_value_size = expected_fs::file_size(
      expected_fs::with_value<mapped_size>(test_result::domain{}), source);
  REQUIRE(domain_value_size.ok());
  REQUIRE(domain_value_size.get().value == std::uintmax_t{5});

  const auto domain_exists = expected_fs::exists(
      expected_fs::with_result(test_result::domain{}), source);
  REQUIRE(domain_exists.ok());
  REQUIRE(domain_exists.get());

  const auto direct_custom = expected_fs::file_size(
      custom_fs{.size = 42, .error_value = 77}, custom_path{});
  REQUIRE(direct_custom.has_value());
  REQUIRE(*direct_custom == std::uintmax_t{42});

  const auto stateful_custom = expected_fs::file_size(
      custom_fs{.size = 123, .error_value = 91}, custom_path{});
  REQUIRE(stateful_custom.has_value());
  REQUIRE(*stateful_custom == std::uintmax_t{123});

  const auto mapped_custom_size =
      expected_fs::file_size(expected_fs::with_error<mapped_error>(
                                 custom_fs{.size = 64, .error_value = 91}),
                             custom_path{});
  REQUIRE(mapped_custom_size.has_value());
  REQUIRE(*mapped_custom_size == std::uintmax_t{64});

  const auto value_custom_size =
      expected_fs::file_size(expected_fs::with_error<mapped_error>(
                                 expected_fs::with_value<mapped_size>(
                                     custom_fs{.size = 65, .error_value = 91})),
                             custom_path{});
  REQUIRE(value_custom_size.has_value());
  REQUIRE(value_custom_size->value == std::uintmax_t{65});

  const auto mapped_custom =
      expected_fs::file_size(expected_fs::with_error<mapped_error>(
                                 custom_fs{.size = 64, .error_value = 91}),
                             custom_path{true});
  REQUIRE_FALSE(mapped_custom.has_value());
  REQUIRE(mapped_custom.error().value == 91);

  const auto policy_custom =
      expected_fs::file_size(expected_fs::with_policy<custom_result_policy>(
                                 custom_fs{.size = 128, .error_value = 93}),
                             custom_path{true});
  REQUIRE_FALSE(policy_custom.has_value());
  REQUIRE(policy_custom.error().value == 93);

  const auto policy_custom_size =
      expected_fs::file_size(expected_fs::with_policy<custom_result_policy>(
                                 custom_fs{.size = 128, .error_value = 93}),
                             custom_path{});
  REQUIRE(policy_custom_size.has_value());
  REQUIRE(*policy_custom_size == std::uintmax_t{128});

  auto mapped_domain_missing = expected_fs::file_size(
      expected_fs::with_error<mapped_error>(test_result::domain{}),
      directory.path() / "missing.txt");
  REQUIRE_FALSE(mapped_domain_missing.ok());
  const auto mapped_domain_error =
      std::move(mapped_domain_missing).error_value();
  REQUIRE(mapped_domain_error.value != 0);

  const auto std_like_size = expected_fs::file_size(
      test_backend::std_like_fs{}, test_backend::std_like_path{});
  REQUIRE(std_like_size.has_value());
  REQUIRE(*std_like_size == std::uintmax_t{512});

  const auto mapped_std_like_missing = expected_fs::file_size(
      expected_fs::with_error<mapped_error>(test_backend::std_like_fs{}),
      test_backend::std_like_path{true});
  REQUIRE_FALSE(mapped_std_like_missing.has_value());
  REQUIRE(mapped_std_like_missing.error().value != 0);

  const auto normalized_size =
      expected_fs::file_size(expected_fs::with_policy<rigid_result_policy>(
                                 test_backend::convertible_fs{}),
                             test_backend::std_like_path{});
  REQUIRE(normalized_size.has_value());
  REQUIRE(*normalized_size == std::uintmax_t{768});

  const auto normalized_missing =
      expected_fs::file_size(expected_fs::with_policy<rigid_result_policy>(
                                 test_backend::convertible_fs{}),
                             test_backend::std_like_path{true});
  REQUIRE_FALSE(normalized_missing.has_value());
  REQUIRE(normalized_missing.error().value != 0);

  const auto aliased_backend_size =
      expected_fs::file_size(alias_backend{.size = 640}, custom_path{});
  REQUIRE(aliased_backend_size.has_value());
  REQUIRE(*aliased_backend_size == std::uintmax_t{640});

  const auto mapped_aliased_backend_missing = expected_fs::file_size(
      expected_fs::with_error<mapped_error>(alias_backend{.size = 640}),
      custom_path{true});
  REQUIRE_FALSE(mapped_aliased_backend_missing.has_value());
  REQUIRE(mapped_aliased_backend_missing.error().value != 0);

  const auto existing_adaptor =
      expected_fs::with_error<mapped_error>(test_backend::std_like_fs{});
  const auto replaced_policy =
      expected_fs::with_policy<custom_result_policy>(existing_adaptor);
  const auto replaced_policy_size =
      expected_fs::file_size(replaced_policy, test_backend::std_like_path{});
  REQUIRE(replaced_policy_size.has_value());
  REQUIRE(*replaced_policy_size == std::uintmax_t{512});

  const auto replaced_domain =
      expected_fs::with_result(test_result::domain{}, existing_adaptor);
  auto replaced_domain_size =
      expected_fs::file_size(replaced_domain, test_backend::std_like_path{});
  REQUIRE(replaced_domain_size.ok());
  REQUIRE(replaced_domain_size.get() == std::uintmax_t{512});

  auto replaced_domain_missing = expected_fs::file_size(
      replaced_domain, test_backend::std_like_path{true});
  REQUIRE_FALSE(replaced_domain_missing.ok());
  REQUIRE(replaced_domain_missing.error_value().value() != 0);

  custom_fs mutable_fs{.size = 256, .error_value = 94};
  const auto owned_mutable_fs =
      expected_fs::with_policy<custom_result_policy>(mutable_fs);
  mutable_fs.size = 300;
  const auto owned_lvalue_size =
      expected_fs::file_size(owned_mutable_fs, custom_path{});
  REQUIRE(owned_lvalue_size.has_value());
  REQUIRE(*owned_lvalue_size == std::uintmax_t{256});

  const auto borrowed_mutable_fs =
      expected_fs::with_policy<custom_result_policy>(std::ref(mutable_fs));
  mutable_fs.size = 384;
  const auto borrowed_lvalue_size =
      expected_fs::file_size(borrowed_mutable_fs, custom_path{});
  REQUIRE(borrowed_lvalue_size.has_value());
  REQUIRE(*borrowed_lvalue_size == std::uintmax_t{384});
}

TEST_CASE("fs_traits ops receive backend state across operation families",
          "[customization][ops]") {
  test_backend::tracking_fs backend;
  const fs::path source{"source"};
  const fs::path destination{"destination"};

  const auto absolute = expected_fs::absolute(backend, source);
  REQUIRE(absolute.has_value());
  REQUIRE(*absolute == backend.root / source);
  REQUIRE(backend.call_count == 1);

  const auto borrowed_absolute = expected_fs::absolute(
      expected_fs::with_error<mapped_error>(std::ref(backend)), source);
  REQUIRE(borrowed_absolute.has_value());
  REQUIRE(*borrowed_absolute == backend.root / source);
  REQUIRE(backend.call_count == 2);

  auto owned =
      expected_fs::with_error<mapped_error>(test_backend::tracking_fs{});
  const auto first_owned_absolute = expected_fs::absolute(owned, source);
  const auto second_owned_absolute = expected_fs::absolute(owned, destination);
  REQUIRE(first_owned_absolute.has_value());
  REQUIRE(second_owned_absolute.has_value());
  REQUIRE(owned.fs.call_count == 2);

  const auto status = expected_fs::status(backend, source);
  REQUIRE(status.has_value());
  REQUIRE(status->type() == fs::file_type::regular);

  const auto regular = expected_fs::is_regular_file(backend, source);
  REQUIRE(regular.has_value());
  REQUIRE(*regular);

  const auto symlink = expected_fs::is_symlink(backend, source);
  REQUIRE(symlink.has_value());
  REQUIRE(*symlink);

  const auto equivalent = expected_fs::equivalent(backend, source, source);
  REQUIRE(equivalent.has_value());
  REQUIRE(*equivalent);

  const auto empty_path = expected_fs::is_empty(backend, source);
  REQUIRE(empty_path.has_value());
  REQUIRE_FALSE(*empty_path);

  const auto size = expected_fs::file_size(backend, source);
  REQUIRE(size.has_value());
  REQUIRE(*size == backend.size);

  backend.write_time = fs::file_time_type::clock::now();
  const auto write_time = expected_fs::last_write_time(backend, source);
  REQUIRE(write_time.has_value());
  const bool returned_write_time_matches = *write_time == backend.write_time;
  REQUIRE(returned_write_time_matches);

  const auto new_write_time = backend.write_time + std::chrono::seconds{1};
  const auto set_write_time =
      expected_fs::last_write_time(backend, source, new_write_time);
  REQUIRE(set_write_time.has_value());
  const bool updated_write_time_matches = backend.write_time == new_write_time;
  REQUIRE(updated_write_time_matches);

  const auto space = expected_fs::space(backend, source);
  REQUIRE(space.has_value());
  REQUIRE(space->capacity == backend.space_value.capacity);
  REQUIRE(space->free == backend.space_value.free);
  REQUIRE(space->available == backend.space_value.available);

  const auto resized =
      expected_fs::resize_file(backend, source, std::uintmax_t{4096});
  REQUIRE(resized.has_value());
  REQUIRE(backend.resized_to == std::uintmax_t{4096});

  const auto copied = expected_fs::copy(backend, source, destination);
  REQUIRE(copied.has_value());
  REQUIRE(backend.last_copy_options == fs::copy_options::none);

  const auto copied_file = expected_fs::copy_file(backend, source, destination);
  REQUIRE(copied_file.has_value());
  REQUIRE(*copied_file);
  REQUIRE(backend.last_copy_file_options == fs::copy_options::none);

  REQUIRE(expected_fs::copy_symlink(backend, source, destination).has_value());
  REQUIRE(
      expected_fs::create_hard_link(backend, source, destination).has_value());
  REQUIRE(
      expected_fs::create_symlink(backend, source, destination).has_value());
  REQUIRE(expected_fs::create_directory_symlink(backend, source, destination)
              .has_value());

  const auto changed_permissions =
      expected_fs::permissions(backend, source, fs::perms::owner_read);
  REQUIRE(changed_permissions.has_value());
  REQUIRE(backend.last_permission_options == fs::perm_options::replace);

  auto made_entry = expected_fs::make_directory_entry(backend, source);
  REQUIRE(made_entry.has_value());
  auto entry = std::move(*made_entry);
  REQUIRE(entry.value == source);

  const auto entry_status = expected_fs::status(backend, entry);
  REQUIRE(entry_status.has_value());
  REQUIRE(entry_status->type() == fs::file_type::directory);

  const auto entry_empty = expected_fs::is_empty(backend, entry);
  REQUIRE(entry_empty.has_value());
  REQUIRE(*entry_empty);

  const auto entry_size = expected_fs::file_size(backend, entry);
  REQUIRE(entry_size.has_value());
  REQUIRE(*entry_size == backend.size + 1);

  const auto entry_links = expected_fs::hard_link_count(backend, entry);
  REQUIRE(entry_links.has_value());
  REQUIRE(*entry_links == std::uintmax_t{3});

  const auto assigned = expected_fs::assign(backend, entry, destination);
  REQUIRE(assigned.has_value());
  REQUIRE(entry.value == destination);

  const auto replaced =
      expected_fs::replace_filename(backend, entry, fs::path{"leaf"});
  REQUIRE(replaced.has_value());
  REQUIRE(entry.value.filename() == "leaf");

  const auto refreshed = expected_fs::refresh(backend, entry);
  REQUIRE(refreshed.has_value());
  REQUIRE(entry.refresh_count == 1);

  auto made_iterator = expected_fs::make_directory_iterator(backend, source);
  REQUIRE(made_iterator.has_value());
  REQUIRE(backend.last_directory_options == fs::directory_options::none);
  auto iterator = *made_iterator;
  const auto incremented = expected_fs::increment(backend, iterator);
  REQUIRE(incremented.has_value());
  REQUIRE(iterator.position == 2);

  auto made_recursive_iterator =
      expected_fs::make_recursive_directory_iterator(backend, source);
  REQUIRE(made_recursive_iterator.has_value());
  REQUIRE(backend.last_recursive_options == fs::directory_options::none);
  auto recursive_iterator = *made_recursive_iterator;
  const auto recursive_incremented =
      expected_fs::increment(backend, recursive_iterator);
  REQUIRE(recursive_incremented.has_value());
  REQUIRE(recursive_iterator.position == 3);

  const auto popped = expected_fs::pop(backend, recursive_iterator);
  REQUIRE(popped.has_value());
  REQUIRE(recursive_iterator.position == 2);
}

TEST_CASE("raw named wrappers receive custom backend state for every form",
          "[customization][ops][raw]") {
  test_backend::raw_fs backend;
  const std::string source{"source"};

  const auto source_path = expected_fs::u8path(backend, source);
  REQUIRE(source_path.value == "raw:source");
  const auto range_path =
      expected_fs::u8path(backend, source.cbegin(), source.cend());
  REQUIRE(range_path.value == "raw:source");

  test_backend::raw_path left{"left"};
  test_backend::raw_path right{"right"};
  expected_fs::swap(backend, left, right);
  REQUIRE(left.value == "right");
  REQUIRE(right.value == "left");
  REQUIRE(expected_fs::hash_value(backend, left)
          == backend.hash_seed + left.value.size());

  using test_backend::raw_file_type;
  using test_backend::raw_status;
  REQUIRE(
      expected_fs::status_known(backend, raw_status{raw_file_type::regular}));
  REQUIRE(expected_fs::exists(backend, raw_status{raw_file_type::regular}));
  REQUIRE(
      expected_fs::is_block_file(backend, raw_status{raw_file_type::block}));
  REQUIRE(expected_fs::is_character_file(backend,
                                         raw_status{raw_file_type::character}));
  REQUIRE(
      expected_fs::is_directory(backend, raw_status{raw_file_type::directory}));
  REQUIRE(expected_fs::is_fifo(backend, raw_status{raw_file_type::fifo}));
  REQUIRE(expected_fs::is_other(backend, raw_status{raw_file_type::other}));
  REQUIRE(expected_fs::is_regular_file(backend,
                                       raw_status{raw_file_type::regular}));
  REQUIRE(expected_fs::is_socket(backend, raw_status{raw_file_type::socket}));
  REQUIRE(expected_fs::is_symlink(backend, raw_status{raw_file_type::symlink}));

  const test_backend::raw_directory_iterator iterator{3};
  REQUIRE(expected_fs::begin(backend, iterator).position == 4);
  REQUIRE(expected_fs::end(backend, iterator).position == -1);

  test_backend::raw_recursive_directory_iterator recursive{
      .position = 4,
      .iterator_options = fs::directory_options::follow_directory_symlink,
      .iterator_depth = 3,
      .pending = true,
  };
  REQUIRE(expected_fs::begin(backend, recursive).position == 6);
  REQUIRE(expected_fs::end(backend, recursive).position == -2);
  REQUIRE(expected_fs::options(backend, recursive)
          == fs::directory_options::follow_directory_symlink);
  REQUIRE(expected_fs::depth(backend, recursive) == 3);
  REQUIRE(expected_fs::recursion_pending(backend, recursive));
  expected_fs::disable_recursion_pending(backend, recursive);
  REQUIRE_FALSE(recursive.pending);

  REQUIRE(backend.call_count == 22);
}

TEST_CASE("explicit tag_invoke has priority over fs_traits ops",
          "[customization][ops]") {
  test_backend::priority_fs backend;
  const fs::path source{"source"};

  const auto direct = expected_fs::file_size(backend, source);
  REQUIRE(direct.has_value());
  REQUIRE(*direct == std::uintmax_t{901});
  REQUIRE(backend.tag_calls == 1);
  REQUIRE(backend.policy_tag_calls == 0);
  REQUIRE(backend.ops_calls == 0);

  const auto adapted = expected_fs::file_size(
      expected_fs::with_error<mapped_error>(std::ref(backend)), source);
  REQUIRE(adapted.has_value());
  REQUIRE(*adapted == std::uintmax_t{902});
  REQUIRE(backend.tag_calls == 1);
  REQUIRE(backend.policy_tag_calls == 1);
  REQUIRE(backend.ops_calls == 0);

  const auto default_copy = expected_fs::copy(
      expected_fs::with_error<mapped_error>(std::ref(backend)), source,
      fs::path{"destination"});
  REQUIRE(default_copy.has_value());
  REQUIRE(backend.default_copy_tag_calls == 1);
  REQUIRE(backend.tagged_copy_options == fs::copy_options::none);
  REQUIRE(backend.copy_ops_calls == 0);

  const auto raw = expected_fs::hash_value(backend, source);
  REQUIRE(raw == std::size_t{905});
  REQUIRE(backend.raw_tag_calls == 1);
  REQUIRE(backend.raw_ops_calls == 0);
}

TEST_CASE("default backend injection re-enters tag_invoke dispatch",
          "[customization][ops]") {
  int tag_calls{};

  const auto result = expected_fs::file_size(
      test_backend::default_injected_path{.tag_calls = &tag_calls});
  REQUIRE(result.has_value());
  REQUIRE(*result == std::uintmax_t{904});
  REQUIRE(tag_calls == 1);
}

TEST_CASE("raw named wrappers preserve std filesystem behavior",
          "[filesystem][raw]") {
  const std::string encoded_path{"folder/file.txt"};
  REQUIRE(expected_fs::u8path(encoded_path) == fs::path{encoded_path});
  REQUIRE(expected_fs::u8path(encoded_path.cbegin(), encoded_path.cend())
          == fs::path{encoded_path});

  fs::path left{"left"};
  fs::path right{"right"};
  expected_fs::swap(left, right);
  REQUIRE(left == fs::path{"right"});
  REQUIRE(right == fs::path{"left"});
  REQUIRE(expected_fs::hash_value(left) == fs::hash_value(left));

  const fs::file_status regular{fs::file_type::regular};
  REQUIRE(expected_fs::status_known(regular));
  REQUIRE_FALSE(expected_fs::status_known(fs::file_status{}));
  REQUIRE(expected_fs::exists(regular));
  REQUIRE_FALSE(expected_fs::is_block_file(regular));
  REQUIRE_FALSE(expected_fs::is_character_file(regular));
  REQUIRE_FALSE(expected_fs::is_directory(regular));
  REQUIRE_FALSE(expected_fs::is_fifo(regular));
  REQUIRE_FALSE(expected_fs::is_other(regular));
  REQUIRE(expected_fs::is_regular_file(regular));
  REQUIRE_FALSE(expected_fs::is_socket(regular));
  REQUIRE_FALSE(expected_fs::is_symlink(regular));

  const temporary_directory directory;
  const auto root = directory.path() / "root";
  const auto nested = root / "nested";
  REQUIRE(fs::create_directories(nested));
  write_text(nested / "file.txt", "data");

  const fs::directory_iterator iterator{root};
  REQUIRE(expected_fs::begin(iterator) == iterator);
  REQUIRE(expected_fs::end(iterator) == fs::directory_iterator{});

  fs::recursive_directory_iterator recursive{root};
  REQUIRE(expected_fs::begin(recursive) == recursive);
  REQUIRE(expected_fs::end(recursive) == fs::recursive_directory_iterator{});
  REQUIRE(expected_fs::options(recursive) == fs::directory_options::none);
  REQUIRE(expected_fs::depth(recursive) == 0);
  REQUIRE(expected_fs::recursion_pending(recursive));
  expected_fs::disable_recursion_pending(recursive);
  REQUIRE_FALSE(expected_fs::recursion_pending(recursive));
}

TEST_CASE("expected_fs wraps common file operations", "[filesystem]") {
  const temporary_directory directory;
  const auto source = directory.path() / "source.txt";
  write_text(source, "hello");

  const auto absolute_path = expected_fs::absolute(source);
  REQUIRE(absolute_path.has_value());
  REQUIRE(absolute_path->is_absolute());

  const auto canonical_path = expected_fs::canonical(source);
  REQUIRE(canonical_path.has_value());
  const auto canonical_matches_source =
      expected_fs::equivalent(*canonical_path, source);
  REQUIRE(canonical_matches_source.has_value());
  REQUIRE(*canonical_matches_source);

  const auto weakly_canonical_path = expected_fs::weakly_canonical(source);
  REQUIRE(weakly_canonical_path.has_value());
  REQUIRE(weakly_canonical_path->is_absolute());

  const auto relative_path = expected_fs::relative(source, directory.path());
  REQUIRE(relative_path.has_value());
  REQUIRE(*relative_path == fs::path{"source.txt"});

  const auto proximate_path = expected_fs::proximate(source, directory.path());
  REQUIRE(proximate_path.has_value());
  REQUIRE(*proximate_path == fs::path{"source.txt"});

  const auto file_status = expected_fs::status(source);
  REQUIRE(file_status.has_value());
  REQUIRE(file_status->type() == fs::file_type::regular);
  REQUIRE(expected_fs::status_known(*file_status));
  REQUIRE(expected_fs::exists(*file_status));
  REQUIRE(expected_fs::is_regular_file(*file_status));
  REQUIRE_FALSE(expected_fs::is_directory(*file_status));
  REQUIRE_FALSE(expected_fs::is_block_file(*file_status));
  REQUIRE_FALSE(expected_fs::is_character_file(*file_status));
  REQUIRE_FALSE(expected_fs::is_fifo(*file_status));
  REQUIRE_FALSE(expected_fs::is_other(*file_status));
  REQUIRE_FALSE(expected_fs::is_socket(*file_status));
  REQUIRE_FALSE(expected_fs::is_symlink(*file_status));

  const auto file_symlink_status = expected_fs::symlink_status(source);
  REQUIRE(file_symlink_status.has_value());
  REQUIRE(file_symlink_status->type() == fs::file_type::regular);

  const auto source_exists = expected_fs::exists(source);
  REQUIRE(source_exists.has_value());
  REQUIRE(*source_exists);

  const auto source_is_regular = expected_fs::is_regular_file(source);
  REQUIRE(source_is_regular.has_value());
  REQUIRE(*source_is_regular);

  const auto source_is_directory = expected_fs::is_directory(source);
  REQUIRE(source_is_directory.has_value());
  REQUIRE_FALSE(*source_is_directory);

  const auto source_is_symlink = expected_fs::is_symlink(source);
  REQUIRE(source_is_symlink.has_value());
  REQUIRE_FALSE(*source_is_symlink);

  const auto source_is_empty = expected_fs::is_empty(source);
  REQUIRE(source_is_empty.has_value());
  REQUIRE_FALSE(*source_is_empty);

  const auto original_size = expected_fs::file_size(source);
  REQUIRE(original_size.has_value());
  REQUIRE(*original_size == std::uintmax_t{5});

  const auto link_count = expected_fs::hard_link_count(source);
  REQUIRE(link_count.has_value());
  REQUIRE(*link_count >= std::uintmax_t{1});

  const auto original_write_time = expected_fs::last_write_time(source);
  REQUIRE(original_write_time.has_value());
  REQUIRE(
      expected_fs::last_write_time(source, *original_write_time).has_value());

  const auto containing_space = expected_fs::space(directory.path());
  REQUIRE(containing_space.has_value());
  REQUIRE(containing_space->capacity > std::uintmax_t{0});

  REQUIRE(expected_fs::resize_file(source, std::uintmax_t{2}).has_value());
  const auto resized_size = expected_fs::file_size(source);
  REQUIRE(resized_size.has_value());
  REQUIRE(*resized_size == std::uintmax_t{2});

  const auto copied =
      expected_fs::copy_file(source, directory.path() / "copy.txt");
  REQUIRE(copied.has_value());
  REQUIRE(*copied);

  REQUIRE(expected_fs::rename(directory.path() / "copy.txt",
                              directory.path() / "renamed.txt")
              .has_value());

  const auto removed_copy =
      expected_fs::remove(directory.path() / "renamed.txt");
  REQUIRE(removed_copy.has_value());
  REQUIRE(*removed_copy);
}

TEST_CASE("expected_fs wraps directory_entry operations", "[filesystem]") {
  const temporary_directory directory;
  const auto first = directory.path() / "first.txt";
  const auto second = directory.path() / "second.txt";
  write_text(first, "first");
  write_text(second, "second");

  auto entry = expected_fs::make_directory_entry(first);
  REQUIRE(entry.has_value());
  REQUIRE(entry->path() == first);

  const auto entry_exists = expected_fs::exists(*entry);
  REQUIRE(entry_exists.has_value());
  REQUIRE(*entry_exists);

  const auto entry_is_regular = expected_fs::is_regular_file(*entry);
  REQUIRE(entry_is_regular.has_value());
  REQUIRE(*entry_is_regular);

  const auto entry_is_directory = expected_fs::is_directory(*entry);
  REQUIRE(entry_is_directory.has_value());
  REQUIRE_FALSE(*entry_is_directory);

  const auto entry_size = expected_fs::file_size(*entry);
  REQUIRE(entry_size.has_value());
  REQUIRE(*entry_size == std::uintmax_t{5});

  const auto entry_links = expected_fs::hard_link_count(*entry);
  REQUIRE(entry_links.has_value());
  REQUIRE(*entry_links >= std::uintmax_t{1});

  const auto entry_write_time = expected_fs::last_write_time(*entry);
  REQUIRE(entry_write_time.has_value());

  const auto entry_status = expected_fs::status(*entry);
  REQUIRE(entry_status.has_value());
  REQUIRE(expected_fs::is_regular_file(*entry_status));

  REQUIRE(expected_fs::replace_filename(*entry, second.filename()).has_value());
  REQUIRE(entry->path() == second);
  REQUIRE(expected_fs::refresh(*entry).has_value());

  REQUIRE(expected_fs::assign(*entry, first).has_value());
  REQUIRE(entry->path() == first);

  const auto missing_path = directory.path() / "missing.txt";
  std::error_code expected_missing_error;
  const fs::directory_entry expected_missing_entry{missing_path,
                                                   expected_missing_error};

  const auto missing_entry = expected_fs::make_directory_entry(missing_path);
  REQUIRE(missing_entry.has_value()
          == !static_cast<bool>(expected_missing_error));
  if (expected_missing_error) {
    REQUIRE(missing_entry.error() == expected_missing_error);
  } else {
    REQUIRE(missing_entry->path() == expected_missing_entry.path());
  }
}

TEST_CASE("expected_fs wraps directory operations", "[filesystem]") {
  const temporary_directory directory;
  const auto source_root = directory.path() / "source";
  const auto nested = source_root / "a" / "b";

  const auto created_nested = expected_fs::create_directories(nested);
  REQUIRE(created_nested.has_value());
  REQUIRE(*created_nested);

  const auto created_nested_again = expected_fs::create_directories(nested);
  REQUIRE(created_nested_again.has_value());
  REQUIRE_FALSE(*created_nested_again);

  write_text(nested / "file.txt", "data");

  const auto copied_tree = expected_fs::copy(
      source_root, directory.path() / "copy", fs::copy_options::recursive);
  REQUIRE(copied_tree.has_value());

  const auto copied_file_exists =
      expected_fs::exists(directory.path() / "copy" / "a" / "b" / "file.txt");
  REQUIRE(copied_file_exists.has_value());
  REQUIRE(*copied_file_exists);

  const auto empty_directory = directory.path() / "empty";
  const auto created_empty = expected_fs::create_directory(empty_directory);
  REQUIRE(created_empty.has_value());
  REQUIRE(*created_empty);

  const auto empty_directory_is_empty = expected_fs::is_empty(empty_directory);
  REQUIRE(empty_directory_is_empty.has_value());
  REQUIRE(*empty_directory_is_empty);

  const auto empty_directory_is_directory =
      expected_fs::is_directory(empty_directory);
  REQUIRE(empty_directory_is_directory.has_value());
  REQUIRE(*empty_directory_is_directory);

  const auto copied_removed =
      expected_fs::remove_all(directory.path() / "copy");
  REQUIRE(copied_removed.has_value());
  REQUIRE(*copied_removed > std::uintmax_t{0});
}

TEST_CASE("expected_fs wraps directory iterators", "[filesystem]") {
  const temporary_directory directory;
  write_text(directory.path() / "a.txt", "a");
  write_text(directory.path() / "b.txt", "b");

  auto iterator = expected_fs::make_directory_iterator(directory.path());
  REQUIRE(iterator.has_value());

  std::set<std::string> names;
  const auto last = expected_fs::end(*iterator);
  while (*iterator != last) {
    names.insert((*iterator)->path().filename().string());
    REQUIRE(expected_fs::increment(*iterator).has_value());
  }

  REQUIRE(names == std::set<std::string>{"a.txt", "b.txt"});

  const auto failed_iterator =
      expected_fs::make_directory_iterator(directory.path() / "a.txt");
  REQUIRE_FALSE(failed_iterator.has_value());
  REQUIRE(failed_iterator.error().value() != 0);
}

TEST_CASE("expected_fs wraps recursive directory iterators", "[filesystem]") {
  const temporary_directory directory;
  const auto root = directory.path() / "root";
  const auto nested = root / "nested";
  REQUIRE(expected_fs::create_directories(nested).has_value());
  write_text(nested / "file.txt", "data");

  auto iterator = expected_fs::make_recursive_directory_iterator(root);
  REQUIRE(iterator.has_value());
  REQUIRE(expected_fs::options(*iterator) == fs::directory_options::none);
  REQUIRE(expected_fs::depth(*iterator) == 0);
  REQUIRE(expected_fs::recursion_pending(*iterator));

  expected_fs::disable_recursion_pending(*iterator);
  REQUIRE_FALSE(expected_fs::recursion_pending(*iterator));
  REQUIRE(expected_fs::increment(*iterator).has_value());
  REQUIRE(*iterator == expected_fs::end(*iterator));

  auto pop_iterator = expected_fs::make_recursive_directory_iterator(root);
  REQUIRE(pop_iterator.has_value());
  const auto recursive_last = expected_fs::end(*pop_iterator);
  while (*pop_iterator != recursive_last
         && expected_fs::depth(*pop_iterator) == 0) {
    REQUIRE(expected_fs::increment(*pop_iterator).has_value());
  }

  REQUIRE(*pop_iterator != recursive_last);
  REQUIRE(expected_fs::depth(*pop_iterator) > 0);
  REQUIRE(expected_fs::pop(*pop_iterator).has_value());
}

TEST_CASE("expected_fs wraps current path and temporary directory queries",
          "[filesystem]") {
  const temporary_directory directory;
  const auto original_path = fs::current_path();
  const current_path_guard restore_original_path{original_path};

  const auto temp_path = expected_fs::temp_directory_path();
  REQUIRE(temp_path.has_value());
  REQUIRE(temp_path->is_absolute());

  REQUIRE(expected_fs::current_path(directory.path()).has_value());

  const auto current = expected_fs::current_path();
  REQUIRE(current.has_value());

  const auto changed_to_test_directory =
      expected_fs::equivalent(*current, directory.path());
  REQUIRE(changed_to_test_directory.has_value());
  REQUIRE(*changed_to_test_directory);
}

TEST_CASE("expected_fs returns error_code for filesystem failures",
          "[filesystem]") {
  const temporary_directory directory;
  const auto missing = directory.path() / "missing.txt";

  const auto missing_size = expected_fs::file_size(missing);
  REQUIRE_FALSE(missing_size.has_value());
  REQUIRE(missing_size.error().value() != 0);

  const auto missing_canonical = expected_fs::canonical(missing);
  REQUIRE_FALSE(missing_canonical.has_value());
  REQUIRE(missing_canonical.error().value() != 0);

  const auto missing_copy =
      expected_fs::copy_file(missing, directory.path() / "copy.txt");
  REQUIRE_FALSE(missing_copy.has_value());
  REQUIRE(missing_copy.error().value() != 0);

  const auto missing_remove = expected_fs::remove(missing);
  REQUIRE(missing_remove.has_value());
  REQUIRE_FALSE(*missing_remove);

  const auto missing_status = expected_fs::status(missing);
  REQUIRE(missing_status.has_value());
  REQUIRE(missing_status->type() == fs::file_type::not_found);

  const auto missing_exists = expected_fs::exists(missing);
  REQUIRE(missing_exists.has_value());
  REQUIRE_FALSE(*missing_exists);

  const auto missing_regular = expected_fs::is_regular_file(missing);
  REQUIRE(missing_regular.has_value());
  REQUIRE_FALSE(*missing_regular);

  const auto regular_file = directory.path() / "regular.txt";
  write_text(regular_file, "x");

  const auto impossible_directory =
      expected_fs::create_directory(regular_file / "child");
  REQUIRE_FALSE(impossible_directory.has_value());
  REQUIRE(impossible_directory.error().value() != 0);

  const auto read_regular_file_as_symlink =
      expected_fs::read_symlink(regular_file);
  REQUIRE_FALSE(read_regular_file_as_symlink.has_value());
  REQUIRE(read_regular_file_as_symlink.error().value() != 0);
}
