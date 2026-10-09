#ifndef EXPECTED_FS_EXPECTED_FS_HPP_
#define EXPECTED_FS_EXPECTED_FS_HPP_

#include <concepts>
#include <cstddef>
#include <cstdint>

#ifdef __has_include
#if __has_include(<expected>)
#include <expected>
#endif
#else
#include <expected>
#endif

#include <filesystem>
#include <functional>
#include <system_error>
#include <type_traits>
#include <utility>

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#if defined(__cpp_lib_format_path) && (__cpp_lib_format_path >= 202403L)
#define EXPECTED_FS_HAS_FORMAT_PATH 1
#else
#define EXPECTED_FS_HAS_FORMAT_PATH 0
#endif
// NOLINTEND(cppcoreguidelines-macro-usage)

#if !defined(__cpp_lib_expected) || (__cpp_lib_expected < 202202L)
#error "expected_fs requires C++23 <expected> support."
#else

namespace expected_fs {
struct std_fs_t {};
inline constexpr std_fs_t std_fs{};

struct std_fs_ops {
  using copy_options = std::filesystem::copy_options;
  using directory_entry = std::filesystem::directory_entry;
  using directory_iterator = std::filesystem::directory_iterator;
  using directory_options = std::filesystem::directory_options;
  using file_status = std::filesystem::file_status;
  using file_time_type = std::filesystem::file_time_type;
  using file_type = std::filesystem::file_type;
  using filesystem_error = std::filesystem::filesystem_error;
  using path = std::filesystem::path;
  using perm_options = std::filesystem::perm_options;
  using perms = std::filesystem::perms;
  using recursive_directory_iterator =
      std::filesystem::recursive_directory_iterator;
  using space_info = std::filesystem::space_info;

  [[nodiscard]] static path
  absolute(const path &value, std::error_code &error) {
    return std::filesystem::absolute(value, error);
  }

  [[nodiscard]] static path
  canonical(const path &value, std::error_code &error) {
    return std::filesystem::canonical(value, error);
  }

  [[nodiscard]] static path
  weakly_canonical(const path &value, std::error_code &error) {
    return std::filesystem::weakly_canonical(value, error);
  }

  [[nodiscard]] static path
  relative(const path &value, std::error_code &error) {
    return std::filesystem::relative(value, error);
  }

  [[nodiscard]] static path
  relative(const path &value, const path &base, std::error_code &error) {
    return std::filesystem::relative(value, base, error);
  }

  [[nodiscard]] static path
  proximate(const path &value, std::error_code &error) {
    return std::filesystem::proximate(value, error);
  }

  [[nodiscard]] static path
  proximate(const path &value, const path &base, std::error_code &error) {
    return std::filesystem::proximate(value, base, error);
  }

  [[nodiscard]] static path
  read_symlink(const path &value, std::error_code &error) {
    return std::filesystem::read_symlink(value, error);
  }

  [[nodiscard]] static path temp_directory_path(std::error_code &error) {
    return std::filesystem::temp_directory_path(error);
  }

  [[nodiscard]] static path current_path(std::error_code &error) {
    return std::filesystem::current_path(error);
  }

  static void current_path(const path &value, std::error_code &error) {
    std::filesystem::current_path(value, error);
  }

  [[nodiscard]] static file_status
  status(const path &value, std::error_code &error) {
    return std::filesystem::status(value, error);
  }

  [[nodiscard]] static file_status
  symlink_status(const path &value, std::error_code &error) {
    return std::filesystem::symlink_status(value, error);
  }

  [[nodiscard]] static bool exists(file_status value) noexcept {
    return std::filesystem::exists(value);
  }

  [[nodiscard]] static bool
  equivalent(const path &left, const path &right, std::error_code &error) {
    return std::filesystem::equivalent(left, right, error);
  }

  [[nodiscard]] static bool is_block_file(file_status value) noexcept {
    return std::filesystem::is_block_file(value);
  }

  [[nodiscard]] static bool is_character_file(file_status value) noexcept {
    return std::filesystem::is_character_file(value);
  }

  [[nodiscard]] static bool is_directory(file_status value) noexcept {
    return std::filesystem::is_directory(value);
  }

  [[nodiscard]] static bool
  is_empty(const path &value, std::error_code &error) {
    return std::filesystem::is_empty(value, error);
  }

  [[nodiscard]] static bool is_fifo(file_status value) noexcept {
    return std::filesystem::is_fifo(value);
  }

  [[nodiscard]] static bool is_other(file_status value) noexcept {
    return std::filesystem::is_other(value);
  }

  [[nodiscard]] static bool is_regular_file(file_status value) noexcept {
    return std::filesystem::is_regular_file(value);
  }

  [[nodiscard]] static bool is_socket(file_status value) noexcept {
    return std::filesystem::is_socket(value);
  }

  [[nodiscard]] static bool is_symlink(file_status value) noexcept {
    return std::filesystem::is_symlink(value);
  }

  [[nodiscard]] static std::uintmax_t
  file_size(const path &value, std::error_code &error) {
    return std::filesystem::file_size(value, error);
  }

  [[nodiscard]] static std::uintmax_t
  hard_link_count(const path &value, std::error_code &error) {
    return std::filesystem::hard_link_count(value, error);
  }

  [[nodiscard]] static file_time_type
  last_write_time(const path &value, std::error_code &error) {
    return std::filesystem::last_write_time(value, error);
  }

  static void last_write_time(const path &value, file_time_type new_time,
                              std::error_code &error) {
    std::filesystem::last_write_time(value, new_time, error);
  }

  [[nodiscard]] static space_info
  space(const path &value, std::error_code &error) {
    return std::filesystem::space(value, error);
  }

  static void copy(const path &from, const path &to, copy_options options,
                   std::error_code &error) {
    std::filesystem::copy(from, to, options, error);
  }

  [[nodiscard]] static bool
  copy_file(const path &from, const path &to, copy_options options,
            std::error_code &error) {
    return std::filesystem::copy_file(from, to, options, error);
  }

  static void
  copy_symlink(const path &from, const path &to, std::error_code &error) {
    std::filesystem::copy_symlink(from, to, error);
  }

  [[nodiscard]] static bool
  create_directory(const path &value, std::error_code &error) {
    return std::filesystem::create_directory(value, error);
  }

  [[nodiscard]] static bool
  create_directory(const path &value, const path &attributes,
                   std::error_code &error) {
    return std::filesystem::create_directory(value, attributes, error);
  }

  [[nodiscard]] static bool
  create_directories(const path &value, std::error_code &error) {
    return std::filesystem::create_directories(value, error);
  }

  static void create_hard_link(const path &target, const path &link,
                               std::error_code &error) {
    std::filesystem::create_hard_link(target, link, error);
  }

  static void
  create_symlink(const path &target, const path &link, std::error_code &error) {
    std::filesystem::create_symlink(target, link, error);
  }

  static void create_directory_symlink(const path &target, const path &link,
                                       std::error_code &error) {
    std::filesystem::create_directory_symlink(target, link, error);
  }

  static void permissions(const path &value, perms new_permissions,
                          perm_options options, std::error_code &error) {
    std::filesystem::permissions(value, new_permissions, options, error);
  }

  [[nodiscard]] static bool remove(const path &value, std::error_code &error) {
    return std::filesystem::remove(value, error);
  }

  [[nodiscard]] static std::uintmax_t
  remove_all(const path &value, std::error_code &error) {
    return std::filesystem::remove_all(value, error);
  }

  static void rename(const path &from, const path &to, std::error_code &error) {
    std::filesystem::rename(from, to, error);
  }

  static void
  resize_file(const path &value, std::uintmax_t size, std::error_code &error) {
    std::filesystem::resize_file(value, size, error);
  }

  // The backend-first overloads form the generic fallible operations protocol.
  // Keeping the backend in every call lets stateful backends implement the same
  // surface without relying on global or static state.
  [[nodiscard]] static path
  absolute(const std_fs_t &, const path &value, std::error_code &error) {
    return absolute(value, error);
  }

  [[nodiscard]] static path
  canonical(const std_fs_t &, const path &value, std::error_code &error) {
    return canonical(value, error);
  }

  [[nodiscard]] static path
  weakly_canonical(const std_fs_t &, const path &value,
                   std::error_code &error) {
    return weakly_canonical(value, error);
  }

  [[nodiscard]] static path
  relative(const std_fs_t &, const path &value, std::error_code &error) {
    return relative(value, error);
  }

  [[nodiscard]] static path relative(const std_fs_t &, const path &value,
                                     const path &base, std::error_code &error) {
    return relative(value, base, error);
  }

  [[nodiscard]] static path
  proximate(const std_fs_t &, const path &value, std::error_code &error) {
    return proximate(value, error);
  }

  [[nodiscard]] static path
  proximate(const std_fs_t &, const path &value, const path &base,
            std::error_code &error) {
    return proximate(value, base, error);
  }

  [[nodiscard]] static path
  read_symlink(const std_fs_t &, const path &value, std::error_code &error) {
    return read_symlink(value, error);
  }

  [[nodiscard]] static path
  temp_directory_path(const std_fs_t &, std::error_code &error) {
    return temp_directory_path(error);
  }

  [[nodiscard]] static path
  current_path(const std_fs_t &, std::error_code &error) {
    return current_path(error);
  }

  static void
  current_path(const std_fs_t &, const path &value, std::error_code &error) {
    current_path(value, error);
  }

  [[nodiscard]] static file_status
  status(const std_fs_t &, const path &value, std::error_code &error) {
    return status(value, error);
  }

  [[nodiscard]] static file_status
  status(const std_fs_t &, const directory_entry &entry,
         std::error_code &error) {
    return entry.status(error);
  }

  [[nodiscard]] static file_status
  symlink_status(const std_fs_t &, const path &value, std::error_code &error) {
    return symlink_status(value, error);
  }

  [[nodiscard]] static file_status
  symlink_status(const std_fs_t &, const directory_entry &entry,
                 std::error_code &error) {
    return entry.symlink_status(error);
  }

  [[nodiscard]] static bool
  exists(const std_fs_t &, file_status value) noexcept {
    return exists(value);
  }

  [[nodiscard]] static bool
  equivalent(const std_fs_t &, const path &left, const path &right,
             std::error_code &error) {
    return equivalent(left, right, error);
  }

  [[nodiscard]] static bool
  is_block_file(const std_fs_t &, file_status value) noexcept {
    return is_block_file(value);
  }

  [[nodiscard]] static bool
  is_character_file(const std_fs_t &, file_status value) noexcept {
    return is_character_file(value);
  }

  [[nodiscard]] static bool
  is_directory(const std_fs_t &, file_status value) noexcept {
    return is_directory(value);
  }

  [[nodiscard]] static bool
  is_empty(const std_fs_t &, const path &value, std::error_code &error) {
    return is_empty(value, error);
  }

  [[nodiscard]] static bool
  is_empty(const std_fs_t &, const directory_entry &entry,
           std::error_code &error) {
    return is_empty(entry.path(), error);
  }

  [[nodiscard]] static bool
  is_fifo(const std_fs_t &, file_status value) noexcept {
    return is_fifo(value);
  }

  [[nodiscard]] static bool
  is_other(const std_fs_t &, file_status value) noexcept {
    return is_other(value);
  }

  [[nodiscard]] static bool
  is_regular_file(const std_fs_t &, file_status value) noexcept {
    return is_regular_file(value);
  }

  [[nodiscard]] static bool
  is_socket(const std_fs_t &, file_status value) noexcept {
    return is_socket(value);
  }

  [[nodiscard]] static bool
  is_symlink(const std_fs_t &, file_status value) noexcept {
    return is_symlink(value);
  }

  [[nodiscard]] static std::uintmax_t
  file_size(const std_fs_t &, const path &value, std::error_code &error) {
    return file_size(value, error);
  }

  [[nodiscard]] static std::uintmax_t
  file_size(const std_fs_t &, const directory_entry &entry,
            std::error_code &error) {
    return entry.file_size(error);
  }

  [[nodiscard]] static std::uintmax_t
  hard_link_count(const std_fs_t &, const path &value, std::error_code &error) {
    return hard_link_count(value, error);
  }

  [[nodiscard]] static std::uintmax_t
  hard_link_count(const std_fs_t &, const directory_entry &entry,
                  std::error_code &error) {
    return entry.hard_link_count(error);
  }

  [[nodiscard]] static file_time_type
  last_write_time(const std_fs_t &, const path &value, std::error_code &error) {
    return last_write_time(value, error);
  }

  [[nodiscard]] static file_time_type
  last_write_time(const std_fs_t &, const directory_entry &entry,
                  std::error_code &error) {
    return entry.last_write_time(error);
  }

  static void last_write_time(const std_fs_t &, const path &value,
                              file_time_type new_time, std::error_code &error) {
    last_write_time(value, new_time, error);
  }

  [[nodiscard]] static space_info
  space(const std_fs_t &, const path &value, std::error_code &error) {
    return space(value, error);
  }

  static void copy(const std_fs_t &, const path &from, const path &to,
                   copy_options options, std::error_code &error) {
    copy(from, to, options, error);
  }

  [[nodiscard]] static bool
  copy_file(const std_fs_t &, const path &from, const path &to,
            copy_options options, std::error_code &error) {
    return copy_file(from, to, options, error);
  }

  static void copy_symlink(const std_fs_t &, const path &from, const path &to,
                           std::error_code &error) {
    copy_symlink(from, to, error);
  }

  [[nodiscard]] static bool
  create_directory(const std_fs_t &, const path &value,
                   std::error_code &error) {
    return create_directory(value, error);
  }

  [[nodiscard]] static bool
  create_directory(const std_fs_t &, const path &value, const path &attributes,
                   std::error_code &error) {
    return create_directory(value, attributes, error);
  }

  [[nodiscard]] static bool
  create_directories(const std_fs_t &, const path &value,
                     std::error_code &error) {
    return create_directories(value, error);
  }

  static void create_hard_link(const std_fs_t &, const path &target,
                               const path &link, std::error_code &error) {
    create_hard_link(target, link, error);
  }

  static void create_symlink(const std_fs_t &, const path &target,
                             const path &link, std::error_code &error) {
    create_symlink(target, link, error);
  }

  static void
  create_directory_symlink(const std_fs_t &, const path &target,
                           const path &link, std::error_code &error) {
    create_directory_symlink(target, link, error);
  }

  static void
  permissions(const std_fs_t &, const path &value, perms new_permissions,
              perm_options options, std::error_code &error) {
    permissions(value, new_permissions, options, error);
  }

  [[nodiscard]] static bool
  remove(const std_fs_t &, const path &value, std::error_code &error) {
    return remove(value, error);
  }

  [[nodiscard]] static std::uintmax_t
  remove_all(const std_fs_t &, const path &value, std::error_code &error) {
    return remove_all(value, error);
  }

  static void rename(const std_fs_t &, const path &from, const path &to,
                     std::error_code &error) {
    rename(from, to, error);
  }

  static void resize_file(const std_fs_t &, const path &value,
                          std::uintmax_t size, std::error_code &error) {
    resize_file(value, size, error);
  }

  [[nodiscard]] static directory_entry
  make_directory_entry(const std_fs_t &, const path &value,
                       std::error_code &error) {
    return {value, error};
  }

  static void assign(const std_fs_t &, directory_entry &entry,
                     const path &value, std::error_code &error) {
    entry.assign(value, error);
  }

  static void replace_filename(const std_fs_t &, directory_entry &entry,
                               const path &value, std::error_code &error) {
    entry.replace_filename(value, error);
  }

  static void
  refresh(const std_fs_t &, directory_entry &entry, std::error_code &error) {
    entry.refresh(error);
  }

  [[nodiscard]] static directory_iterator
  make_directory_iterator(const std_fs_t &, const path &value,
                          directory_options options, std::error_code &error) {
    return {value, options, error};
  }

  static void increment(const std_fs_t &, directory_iterator &iterator,
                        std::error_code &error) {
    iterator.increment(error);
  }

  [[nodiscard]] static recursive_directory_iterator
  make_recursive_directory_iterator(const std_fs_t &, const path &value,
                                    directory_options options,
                                    std::error_code &error) {
    return {value, options, error};
  }

  static void
  increment(const std_fs_t &, recursive_directory_iterator &iterator,
            std::error_code &error) {
    iterator.increment(error);
  }

  static void pop(const std_fs_t &, recursive_directory_iterator &iterator,
                  std::error_code &error) {
    iterator.pop(error);
  }

  [[nodiscard]] static bool
  status_known(const std_fs_t &, file_status value) noexcept {
    return status_known(value);
  }

#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4996) // deprecated u8path
#endif
  template <class Source>
    requires requires(Source &&source) {
      std::filesystem::u8path(std::forward<Source>(source));
    }
  [[nodiscard]] static path u8path(const std_fs_t &, Source &&source) {
    return std::filesystem::u8path(std::forward<Source>(source));
  }

  template <class InputIterator>
    requires requires(InputIterator first, InputIterator last) {
      std::filesystem::u8path(first, last);
    }
  [[nodiscard]] static path
  u8path(const std_fs_t &, InputIterator first, InputIterator last) {
    return std::filesystem::u8path(first, last);
  }
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic pop
#elifdef _MSC_VER
#pragma warning(pop)
#endif

  static void swap(const std_fs_t &, path &left, path &right) noexcept {
    swap(left, right);
  }

  [[nodiscard]] static std::size_t
  hash_value(const std_fs_t &, const path &value) noexcept {
    return hash_value(value);
  }

  [[nodiscard]] static directory_iterator
  begin(const std_fs_t &, directory_iterator iterator) noexcept {
    return begin(std::move(iterator));
  }

  [[nodiscard]] static directory_iterator
  end(const std_fs_t &, directory_iterator iterator) noexcept {
    return end(std::move(iterator));
  }

  [[nodiscard]] static directory_options
  options(const std_fs_t &,
          const recursive_directory_iterator &iterator) noexcept {
    return iterator.options();
  }

  [[nodiscard]] static int
  depth(const std_fs_t &,
        const recursive_directory_iterator &iterator) noexcept {
    return iterator.depth();
  }

  [[nodiscard]] static bool
  recursion_pending(const std_fs_t &,
                    const recursive_directory_iterator &iterator) noexcept {
    return iterator.recursion_pending();
  }

  static void
  disable_recursion_pending(const std_fs_t &,
                            recursive_directory_iterator &iterator) noexcept {
    iterator.disable_recursion_pending();
  }

  [[nodiscard]] static recursive_directory_iterator
  begin(const std_fs_t &, recursive_directory_iterator iterator) noexcept {
    return begin(std::move(iterator));
  }

  [[nodiscard]] static recursive_directory_iterator
  end(const std_fs_t &, recursive_directory_iterator iterator) noexcept {
    return end(std::move(iterator));
  }

  [[nodiscard]] static directory_iterator
  begin(directory_iterator iterator) noexcept {
    return std::filesystem::begin(std::move(iterator));
  }

  [[nodiscard]] static directory_iterator
  end(directory_iterator iterator) noexcept {
    return std::filesystem::end(std::move(iterator));
  }

  [[nodiscard]] static recursive_directory_iterator
  begin(recursive_directory_iterator iterator) noexcept {
    return std::filesystem::begin(std::move(iterator));
  }

  [[nodiscard]] static recursive_directory_iterator
  end(recursive_directory_iterator iterator) noexcept {
    return std::filesystem::end(std::move(iterator));
  }

  static void swap(path &left, path &right) noexcept { left.swap(right); }

  [[nodiscard]] static std::size_t hash_value(const path &value) noexcept {
    return std::filesystem::hash_value(value);
  }

  [[nodiscard]] static bool status_known(file_status value) noexcept {
    return std::filesystem::status_known(value);
  }
};

template <class Fs>
struct fs_traits;

template <>
struct fs_traits<std_fs_t> {
  using ops = std_fs_ops;

  using copy_options = ops::copy_options;
  using directory_entry = ops::directory_entry;
  using directory_iterator = ops::directory_iterator;
  using directory_options = ops::directory_options;
  using file_status = ops::file_status;
  using file_time_type = ops::file_time_type;
  using file_type = ops::file_type;
  using filesystem_error = ops::filesystem_error;
  using path = ops::path;
  using perm_options = ops::perm_options;
  using perms = ops::perms;
  using recursive_directory_iterator = ops::recursive_directory_iterator;
  using space_info = ops::space_info;
};

template <class Fs>
using fs_copy_options_t = fs_traits<std::remove_cvref_t<Fs>>::copy_options;

template <class Fs>
using fs_directory_entry_t =
    fs_traits<std::remove_cvref_t<Fs>>::directory_entry;

template <class Fs>
using fs_directory_iterator_t =
    fs_traits<std::remove_cvref_t<Fs>>::directory_iterator;

template <class Fs>
using fs_directory_options_t =
    fs_traits<std::remove_cvref_t<Fs>>::directory_options;

template <class Fs>
using fs_file_status_t = fs_traits<std::remove_cvref_t<Fs>>::file_status;

template <class Fs>
using fs_file_time_type_t = fs_traits<std::remove_cvref_t<Fs>>::file_time_type;

template <class Fs>
using fs_file_type_t = fs_traits<std::remove_cvref_t<Fs>>::file_type;

template <class Fs>
using fs_filesystem_error_t =
    fs_traits<std::remove_cvref_t<Fs>>::filesystem_error;

template <class Fs>
using fs_path_t = fs_traits<std::remove_cvref_t<Fs>>::path;

template <class Fs>
using fs_perm_options_t = fs_traits<std::remove_cvref_t<Fs>>::perm_options;

template <class Fs>
using fs_perms_t = fs_traits<std::remove_cvref_t<Fs>>::perms;

template <class Fs>
using fs_recursive_directory_iterator_t =
    fs_traits<std::remove_cvref_t<Fs>>::recursive_directory_iterator;

template <class Fs>
using fs_space_info_t = fs_traits<std::remove_cvref_t<Fs>>::space_info;

template <class Fs>
using fs_ops_t = fs_traits<std::remove_cvref_t<Fs>>::ops;

using copy_options = fs_copy_options_t<std_fs_t>;
using directory_entry = fs_directory_entry_t<std_fs_t>;
using directory_iterator = fs_directory_iterator_t<std_fs_t>;
using directory_options = fs_directory_options_t<std_fs_t>;
using file_status = fs_file_status_t<std_fs_t>;
using file_time_type = fs_file_time_type_t<std_fs_t>;
using file_type = fs_file_type_t<std_fs_t>;
using filesystem_error = fs_filesystem_error_t<std_fs_t>;
using path = fs_path_t<std_fs_t>;
using perm_options = fs_perm_options_t<std_fs_t>;
using perms = fs_perms_t<std_fs_t>;
using recursive_directory_iterator =
    fs_recursive_directory_iterator_t<std_fs_t>;
using space_info = fs_space_info_t<std_fs_t>;

void tag_invoke();

struct std_result_domain {};
inline constexpr std_result_domain std_result{};

template <class Domain>
struct result_domain_traits;

template <>
struct result_domain_traits<std_result_domain> {
  template <class T, class E>
  using result = std::expected<T, E>;
};

template <class Domain, class T, class E>
using result_t =
    result_domain_traits<std::remove_cvref_t<Domain>>::template result<T, E>;

template <class T, class E = std::error_code>
using expected = result_t<std_result_domain, T, E>;

inline constexpr bool has_format_path = EXPECTED_FS_HAS_FORMAT_PATH != 0;

struct result_success_t {
  template <class Domain, class T, class E, class Value>
  [[nodiscard]] constexpr decltype(auto)
  operator()(Domain &&domain, std::type_identity<T> value_type,
             std::type_identity<E> error_type, Value &&value) const {
    return tag_invoke(*this, std::forward<Domain>(domain), value_type,
                      error_type, std::forward<Value>(value));
  }

  template <class Domain, class E>
  [[nodiscard]] constexpr decltype(auto)
  operator()(Domain &&domain, std::type_identity<void> value_type,
             std::type_identity<E> error_type) const {
    return tag_invoke(*this, std::forward<Domain>(domain), value_type,
                      error_type);
  }
};

inline constexpr result_success_t result_success{};

struct result_failure_t {
  template <class Domain, class T, class E, class Err>
  [[nodiscard]] constexpr decltype(auto)
  operator()(Domain &&domain, std::type_identity<T> value_type,
             std::type_identity<E> error_type, Err &&error) const {
    return tag_invoke(*this, std::forward<Domain>(domain), value_type,
                      error_type, std::forward<Err>(error));
  }
};

inline constexpr result_failure_t result_failure{};

template <class T, class E, class Value>
  requires(!std::is_void_v<T>)
[[nodiscard]] inline auto
tag_invoke(result_success_t, std_result_domain, std::type_identity<T>,
           std::type_identity<E>, Value &&value)
    -> result_t<std_result_domain, T, E> {
  return result_t<std_result_domain, T, E>{std::forward<Value>(value)};
}

template <class E>
[[nodiscard]] inline auto
tag_invoke(result_success_t, std_result_domain, std::type_identity<void>,
           std::type_identity<E>) -> result_t<std_result_domain, void, E> {
  return {};
}

template <class T, class E, class Err>
[[nodiscard]] inline auto
tag_invoke(result_failure_t, std_result_domain, std::type_identity<T>,
           std::type_identity<E>, Err &&error)
    -> result_t<std_result_domain, T, E> {
  return std::unexpected(E{std::forward<Err>(error)});
}

namespace detail {
struct natural_value_t {};

template <class RequestedValue, class OperationValue>
using policy_value_t =
    std::conditional_t<std::is_same_v<RequestedValue, natural_value_t>,
                       OperationValue, RequestedValue>;
} // namespace detail

template <class Domain, class E = std::error_code,
          class Value = detail::natural_value_t>
struct result_policy {
  using domain_type = std::remove_cvref_t<Domain>;
  using error_type = E;
  using value_type = Value;

  static_assert(std::is_empty_v<domain_type>,
                "expected_fs result domains must be stateless.");
  static_assert(std::default_initializable<domain_type>,
                "expected_fs result domains must be default-constructible.");

  template <class T>
  using result = result_t<domain_type, detail::policy_value_t<Value, T>, E>;

  template <class T>
  [[nodiscard]] static result<T> success(T value) {
    using result_value_type = detail::policy_value_t<Value, T>;

    if constexpr (std::is_void_v<result_value_type>) {
      return result_success(domain_type{}, std::type_identity<void>{},
                            std::type_identity<E>{});
    } else if constexpr (std::is_same_v<Value, detail::natural_value_t>) {
      return result_success(domain_type{},
                            std::type_identity<result_value_type>{},
                            std::type_identity<E>{}, std::move(value));
    } else {
      return result_success(
          domain_type{}, std::type_identity<result_value_type>{},
          std::type_identity<E>{}, result_value_type{std::move(value)});
    }
  }

  [[nodiscard]] static result<void> success() {
    using result_value_type = detail::policy_value_t<Value, void>;

    if constexpr (std::is_void_v<result_value_type>) {
      return result_success(domain_type{}, std::type_identity<void>{},
                            std::type_identity<E>{});
    } else {
      return result_success(domain_type{},
                            std::type_identity<result_value_type>{},
                            std::type_identity<E>{}, result_value_type{});
    }
  }

  template <class T, class Err>
  [[nodiscard]] static result<T> failure(Err &&error) {
    using result_value_type = detail::policy_value_t<Value, T>;

    return result_failure(domain_type{},
                          std::type_identity<result_value_type>{},
                          std::type_identity<E>{}, std::forward<Err>(error));
  }
};

template <class E = std::error_code, class Value = detail::natural_value_t>
using std_result_policy = result_policy<std_result_domain, E, Value>;

template <class Fs, class Policy>
struct result_adaptor {
  using fs_type = Fs;
  using policy_type = Policy;

  // Explicit std::ref/std::cref adaptors intentionally retain a reference.
  // NOLINTNEXTLINE(cppcoreguidelines-avoid-const-or-ref-data-members)
  [[no_unique_address]] Fs fs;
};

template <class Fs, class Policy>
struct fs_traits<result_adaptor<Fs, Policy>>
    : fs_traits<std::remove_cvref_t<Fs>> {};

template <class E>
struct with_error_t;

template <class T>
struct with_value_t;

struct with_result_t;

template <class Policy>
struct with_policy_t;

namespace detail {
void tag_invoke();

template <class Tag, class... Args>
concept tag_invocable =
    requires { tag_invoke(std::declval<Tag>(), std::declval<Args>()...); };

template <class Tag, class... Args>
using tag_invoke_result_t =
    decltype(tag_invoke(std::declval<Tag>(), std::declval<Args>()...));

template <class T>
concept adaptor_closure =
    requires { typename std::remove_cvref_t<T>::expected_fs_adaptor_closure; };

template <class T>
concept result_domain =
    std::is_empty_v<std::remove_cvref_t<T>>
    && std::default_initializable<std::remove_cvref_t<T>>
    && requires {
         typename result_domain_traits<std::remove_cvref_t<T>>::template result<
             int, std::error_code>;
       };

template <class T>
struct is_result_adaptor : std::false_type {};

template <class Fs, class Policy>
struct is_result_adaptor<result_adaptor<Fs, Policy>> : std::true_type {};

template <class T>
concept result_adaptor_instance =
    is_result_adaptor<std::remove_cvref_t<T>>::value;

template <class T>
struct result_adaptor_traits;

template <class Fs, class Policy>
struct result_adaptor_traits<result_adaptor<Fs, Policy>> {
  using fs_type = Fs;
  using policy_type = Policy;
};

template <class Adaptor>
using adaptor_fs_t =
    result_adaptor_traits<std::remove_cvref_t<Adaptor>>::fs_type;

template <class Adaptor>
using adaptor_policy_t =
    result_adaptor_traits<std::remove_cvref_t<Adaptor>>::policy_type;

template <class Fs>
using forwarding_fs_t = std::conditional_t<std::is_lvalue_reference_v<Fs>, Fs,
                                           std::remove_cvref_t<Fs>>;

template <class Fs>
using default_result_adaptor_t =
    result_adaptor<forwarding_fs_t<Fs>, std_result_policy<>>;

template <class Tag, class Fs, class... Args>
concept policy_tag_invocable =
    tag_invocable<Tag, default_result_adaptor_t<Fs>, Args...>;

using default_fs_lvalue_t = const std_fs_t &;

template <class Fs>
using persistent_fs_t = std::unwrap_ref_decay_t<Fs>;

template <class Fs>
[[nodiscard]] constexpr auto
store_persistent_fs(Fs &&fs) -> persistent_fs_t<Fs> {
  return std::forward<Fs>(fs);
}

template <class Fs>
[[nodiscard]] constexpr decltype(auto) forward_stored_fs(Fs &fs) noexcept {
  if constexpr (std::is_lvalue_reference_v<Fs>) {
    return fs;
  } else {
    return std::move(fs);
  }
}

template <class Policy, class Fs>
[[nodiscard]] constexpr auto make_persistent_adaptor(Fs &&fs) {
  using fs_type = std::remove_cvref_t<Fs>;

  if constexpr (std::same_as<fs_type, std_fs_t>) {
    return result_adaptor<std_fs_t, Policy>{std_fs};
  } else {
    using stored_fs_type = persistent_fs_t<Fs>;
    return result_adaptor<stored_fs_type, Policy>{
        store_persistent_fs(std::forward<Fs>(fs)),
    };
  }
}

template <class Policy, class Fs, class OldPolicy>
[[nodiscard]] constexpr auto
rebind_adaptor(result_adaptor<Fs, OldPolicy> adaptor) {
  return result_adaptor<Fs, Policy>{
      forward_stored_fs<Fs>(adaptor.fs),
  };
}

template <class Fs>
concept filesystem_traits =
    requires { typename fs_traits<std::remove_cvref_t<Fs>>::path; };

template <class Fs>
concept filesystem_file_status = filesystem_traits<Fs> && requires {
  typename fs_traits<std::remove_cvref_t<Fs>>::file_status;
};

template <class Fs>
concept filesystem_directory_iterator = filesystem_traits<Fs> && requires {
  typename fs_traits<std::remove_cvref_t<Fs>>::directory_iterator;
};

template <class Fs>
concept filesystem_recursive_directory_iterator =
    filesystem_traits<Fs> && requires {
      typename fs_traits<std::remove_cvref_t<Fs>>::recursive_directory_iterator;
    };

template <class... Args>
struct starts_with_explicit_dispatch_argument : std::false_type {};

template <class T>
[[nodiscard]] consteval bool is_explicit_dispatch_argument() {
  if constexpr (adaptor_closure<T> || result_adaptor_instance<T>) {
    return true;
  } else {
    return filesystem_traits<T>;
  }
}

template <class First, class... Rest>
struct starts_with_explicit_dispatch_argument<First, Rest...>
    : std::bool_constant<is_explicit_dispatch_argument<First>()> {};

template <class... Args>
inline constexpr bool starts_with_explicit_dispatch_argument_v =
    starts_with_explicit_dispatch_argument<Args...>::value;

void ops_invoke();
void raw_ops_invoke();

template <class Adaptor>
using adaptor_lvalue_t = std::remove_reference_t<Adaptor> &;

template <class Fs>
using backend_lvalue_t = std::remove_reference_t<Fs> &;

template <class Tag, class Fs, class... Args>
concept raw_ops_invocable = requires {
  raw_ops_invoke(std::declval<Tag>(), std::declval<backend_lvalue_t<Fs>>(),
                 std::declval<Args>()...);
};

template <class Tag, class Fs, class... Args>
  requires raw_ops_invocable<Tag, Fs, Args...>
[[nodiscard]] constexpr decltype(auto)
invoke_raw_ops(Tag tag, Fs &fs, Args &&...args) noexcept(
    noexcept(raw_ops_invoke(tag, fs, std::forward<Args>(args)...))) {
  return raw_ops_invoke(tag, fs, std::forward<Args>(args)...);
}

template <class Tag, class Adaptor, class... Args>
concept ops_invocable = requires {
  ops_invoke(std::declval<Tag>(), std::declval<adaptor_lvalue_t<Adaptor>>(),
             std::declval<Args>()...);
};

template <class Tag, class Adaptor, class... Args>
  requires ops_invocable<Tag, Adaptor, Args...>
[[nodiscard]] constexpr decltype(auto)
invoke_ops(Tag tag, Adaptor &adaptor, Args &&...args) {
  return ops_invoke(tag, adaptor, std::forward<Args>(args)...);
}

template <class Derived>
struct cpo_base {
private:
  constexpr cpo_base() = default;

public:
  template <class... Args>
    requires tag_invocable<Derived, Args...>
  [[nodiscard]] constexpr decltype(auto) operator()(Args &&...args) const
      noexcept(noexcept(tag_invoke(Derived{}, std::forward<Args>(args)...))) {
    return tag_invoke(Derived{}, std::forward<Args>(args)...);
  }

  template <class Adaptor, class... Args>
    requires adaptor_closure<Adaptor>
          && (!tag_invocable<Derived, Adaptor, Args...>)
          && requires(Adaptor &&adaptor, Args &&...args) {
               Derived{}(std::forward<Adaptor>(adaptor)(std_fs),
                         std::forward<Args>(args)...);
             }
  [[nodiscard]] constexpr decltype(auto)
  operator()(Adaptor &&adaptor, Args &&...args) const
      noexcept(noexcept(Derived{}(std::forward<Adaptor>(adaptor)(std_fs),
                                  std::forward<Args>(args)...))) {
    return Derived{}(std::forward<Adaptor>(adaptor)(std_fs),
                     std::forward<Args>(args)...);
  }

  template <class Adaptor, class... Args>
    requires result_adaptor_instance<Adaptor>
          && (!tag_invocable<Derived, Adaptor, Args...>)
          && ops_invocable<Derived, Adaptor, Args...>
  // The stored backend is deliberately observed as an lvalue so state changes
  // affect the owned or explicitly borrowed object rather than a temporary.
  // NOLINTBEGIN(cppcoreguidelines-missing-std-forward)
  [[nodiscard]] constexpr decltype(auto)
  operator()(Adaptor &&adaptor, Args &&...args) const
      noexcept(noexcept(invoke_ops(Derived{}, adaptor,
                                   std::forward<Args>(args)...))) {
    return invoke_ops(Derived{}, adaptor, std::forward<Args>(args)...);
  }
  // NOLINTEND(cppcoreguidelines-missing-std-forward)

  template <class Fs, class... Args>
    requires(!adaptor_closure<Fs>)
         && (!result_adaptor_instance<Fs>)
         && (!tag_invocable<Derived, Fs, Args...>)
         && policy_tag_invocable<Derived, Fs, Args...>
  [[nodiscard]] constexpr decltype(auto)
  operator()(Fs &&fs, Args &&...args) const {
    default_result_adaptor_t<Fs> adaptor{
        std::forward<Fs>(fs),
    };
    return tag_invoke(Derived{}, std::move(adaptor),
                      std::forward<Args>(args)...);
  }

  template <class Fs, class... Args>
    requires(!adaptor_closure<Fs>)
         && (!result_adaptor_instance<Fs>)
         && filesystem_traits<Fs>
         && (!tag_invocable<Derived, Fs, Args...>)
         && (!policy_tag_invocable<Derived, Fs, Args...>)
         && raw_ops_invocable<Derived, Fs, Args...>
  // The selected backend is deliberately observed as an lvalue for the
  // duration of the operation, matching result-adaptor dispatch.
  // NOLINTBEGIN(cppcoreguidelines-missing-std-forward)
  [[nodiscard]] constexpr decltype(auto)
  operator()(Fs &&fs, Args &&...args) const
      noexcept(noexcept(invoke_raw_ops(Derived{}, fs,
                                       std::forward<Args>(args)...))) {
    return invoke_raw_ops(Derived{}, fs, std::forward<Args>(args)...);
  }
  // NOLINTEND(cppcoreguidelines-missing-std-forward)

  template <class Fs, class... Args>
    requires(!adaptor_closure<Fs>)
         && (!result_adaptor_instance<Fs>)
         && filesystem_traits<Fs>
         && (!tag_invocable<Derived, Fs, Args...>)
         && (!policy_tag_invocable<Derived, Fs, Args...>)
         && (!raw_ops_invocable<Derived, Fs, Args...>)
         && ops_invocable<Derived, default_result_adaptor_t<Fs>, Args...>
  [[nodiscard]] constexpr decltype(auto)
  operator()(Fs &&fs, Args &&...args) const {
    default_result_adaptor_t<Fs> adaptor{
        std::forward<Fs>(fs),
    };
    return invoke_ops(Derived{}, adaptor, std::forward<Args>(args)...);
  }

  template <class... Args>
    requires(!tag_invocable<Derived, Args...>)
         && (!starts_with_explicit_dispatch_argument_v<Args...>)
         && (tag_invocable<Derived, default_fs_lvalue_t, Args...>
             || policy_tag_invocable<Derived, default_fs_lvalue_t, Args...>
             || raw_ops_invocable<Derived, default_fs_lvalue_t, Args...>
             || ops_invocable<Derived,
                              default_result_adaptor_t<default_fs_lvalue_t>,
                              Args...>)
  [[nodiscard]] constexpr decltype(auto) operator()(Args &&...args) const
      noexcept(noexcept(Derived{}(std_fs, std::forward<Args>(args)...))) {
    return Derived{}(std_fs, std::forward<Args>(args)...);
  }

  friend Derived;
};

template <class Policy, class T>
[[nodiscard]] inline auto make_result(T value, const std::error_code &error)
    -> Policy::template result<T> {
  if (error) {
    return Policy::template failure<T>(error);
  }

  return Policy::template success<T>(std::move(value));
}

template <class Policy>
[[nodiscard]] inline auto
make_result(const std::error_code &error) -> Policy::template result<void> {
  if (error) {
    return Policy::template failure<void>(error);
  }

  return Policy::success();
}

} // namespace detail

template <class Domain, class E, class Value = detail::natural_value_t>
struct result_domain_closure {
  using expected_fs_adaptor_closure = void;
  using domain_type = std::remove_cvref_t<Domain>;
  using error_type = E;
  using value_type = Value;

  static_assert(std::is_empty_v<domain_type>,
                "expected_fs result domains must be stateless.");
  static_assert(std::default_initializable<domain_type>,
                "expected_fs result domains must be default-constructible.");

  template <class Fs, class OldPolicy>
  [[nodiscard]] constexpr auto
  operator()(result_adaptor<Fs, OldPolicy> adaptor) const {
    using policy = result_policy<domain_type, E, Value>;
    return detail::rebind_adaptor<policy>(std::move(adaptor));
  }

  template <class Fs>
    requires(!detail::result_adaptor_instance<Fs>)
  [[nodiscard]] constexpr auto operator()(Fs &&fs) const {
    using policy = result_policy<domain_type, E, Value>;
    return detail::make_persistent_adaptor<policy>(std::forward<Fs>(fs));
  }
};

template <class E>
struct with_error_t {
  using expected_fs_adaptor_closure = void;
  using error_type = E;

  template <class Domain>
    requires detail::result_domain<Domain>
  [[nodiscard]] constexpr auto operator()(Domain &&) const {
    return result_domain_closure<std::remove_cvref_t<Domain>, E>{};
  }

  template <class Domain, class OldE, class Value>
  [[nodiscard]] constexpr auto
  operator()(const result_domain_closure<Domain, OldE, Value> &) const {
    return result_domain_closure<Domain, E, Value>{};
  }

  template <class Value>
  [[nodiscard]] constexpr auto operator()(const with_value_t<Value> &) const {
    return result_domain_closure<std_result_domain, E, Value>{};
  }

  template <class Fs, class Domain, class OldE, class Value>
  [[nodiscard]] constexpr auto operator()(
      result_adaptor<Fs, result_policy<Domain, OldE, Value>> adaptor) const {
    using policy = result_policy<Domain, E, Value>;
    return detail::rebind_adaptor<policy>(std::move(adaptor));
  }

  template <class Fs>
    requires(!detail::result_domain<Fs>)
         && (!detail::adaptor_closure<Fs>)
         && (!detail::result_adaptor_instance<Fs>)
  [[nodiscard]] constexpr auto operator()(Fs &&fs) const {
    using policy = std_result_policy<E>;
    return detail::make_persistent_adaptor<policy>(std::forward<Fs>(fs));
  }
};

template <class E>
inline constexpr with_error_t<E> with_error{};

template <class Value>
struct with_value_t {
  using expected_fs_adaptor_closure = void;
  using value_type = Value;

  template <class Domain>
    requires detail::result_domain<Domain>
  [[nodiscard]] constexpr auto operator()(Domain &&) const {
    return result_domain_closure<std::remove_cvref_t<Domain>, std::error_code,
                                 Value>{};
  }

  template <class Domain, class E, class OldValue>
  [[nodiscard]] constexpr auto
  operator()(const result_domain_closure<Domain, E, OldValue> &) const {
    return result_domain_closure<Domain, E, Value>{};
  }

  template <class E>
  [[nodiscard]] constexpr auto operator()(const with_error_t<E> &) const {
    return result_domain_closure<std_result_domain, E, Value>{};
  }

  template <class Fs, class Domain, class E, class OldValue>
  [[nodiscard]] constexpr auto operator()(
      result_adaptor<Fs, result_policy<Domain, E, OldValue>> adaptor) const {
    using policy = result_policy<Domain, E, Value>;
    return detail::rebind_adaptor<policy>(std::move(adaptor));
  }

  template <class Fs>
    requires(!detail::result_domain<Fs>)
         && (!detail::adaptor_closure<Fs>)
         && (!detail::result_adaptor_instance<Fs>)
  [[nodiscard]] constexpr auto operator()(Fs &&fs) const {
    using policy = std_result_policy<std::error_code, Value>;
    return detail::make_persistent_adaptor<policy>(std::forward<Fs>(fs));
  }
};

template <class Value>
inline constexpr with_value_t<Value> with_value{};

struct with_result_t {
  using expected_fs_adaptor_closure = void;

  template <class Domain>
    requires detail::result_domain<Domain>
  [[nodiscard]] constexpr auto operator()(Domain &&) const {
    return result_domain_closure<std::remove_cvref_t<Domain>,
                                 std::error_code>{};
  }

  template <class Domain, class Fs>
    requires detail::result_domain<Domain>
  [[nodiscard]] constexpr auto operator()(Domain &&, Fs &&fs) const {
    return result_domain_closure<std::remove_cvref_t<Domain>,
                                 std::error_code>{}(std::forward<Fs>(fs));
  }
};

inline constexpr with_result_t with_result{};

template <class Policy>
struct with_policy_t {
  using expected_fs_adaptor_closure = void;

  template <class Fs, class OldPolicy>
  [[nodiscard]] constexpr auto
  operator()(result_adaptor<Fs, OldPolicy> adaptor) const {
    return detail::rebind_adaptor<Policy>(std::move(adaptor));
  }

  template <class Fs>
    requires(!detail::result_adaptor_instance<Fs>)
  [[nodiscard]] constexpr auto operator()(Fs &&fs) const {
    return detail::make_persistent_adaptor<Policy>(std::forward<Fs>(fs));
  }
};

template <class Policy>
inline constexpr with_policy_t<Policy> with_policy{};

#define EXPECTED_FS_DEFINE_CPO(name)                                           \
  struct name##_t : detail::cpo_base<name##_t> {                               \
    constexpr name##_t() = default;                                            \
  };                                                                           \
  inline constexpr name##_t name {}

#define EXPECTED_FS_DEFINE_ACTION_CPO(name)                                    \
  struct name##_t : detail::cpo_base<name##_t> {                               \
  private:                                                                     \
    using base = detail::cpo_base<name##_t>;                                   \
                                                                               \
  public:                                                                      \
    constexpr name##_t() = default;                                            \
                                                                               \
    template <class... Args>                                                   \
      requires requires(const base &callable, Args &&...args) {                \
        callable(std::forward<Args>(args)...);                                 \
      }                                                                        \
    constexpr decltype(auto) operator()(Args &&...args) const noexcept(        \
        noexcept(std::declval<const base &>()(std::forward<Args>(args)...))) { \
      return static_cast<const base &>(*this)(std::forward<Args>(args)...);    \
    }                                                                          \
  };                                                                           \
  inline constexpr name##_t name {}

EXPECTED_FS_DEFINE_CPO(absolute);
EXPECTED_FS_DEFINE_CPO(canonical);
EXPECTED_FS_DEFINE_CPO(weakly_canonical);
EXPECTED_FS_DEFINE_CPO(relative);
EXPECTED_FS_DEFINE_CPO(proximate);
EXPECTED_FS_DEFINE_CPO(read_symlink);
EXPECTED_FS_DEFINE_CPO(temp_directory_path);
EXPECTED_FS_DEFINE_CPO(current_path);
EXPECTED_FS_DEFINE_CPO(status);
EXPECTED_FS_DEFINE_CPO(symlink_status);
EXPECTED_FS_DEFINE_CPO(exists);
EXPECTED_FS_DEFINE_CPO(equivalent);
EXPECTED_FS_DEFINE_CPO(is_block_file);
EXPECTED_FS_DEFINE_CPO(is_character_file);
EXPECTED_FS_DEFINE_CPO(is_directory);
EXPECTED_FS_DEFINE_CPO(is_empty);
EXPECTED_FS_DEFINE_CPO(is_fifo);
EXPECTED_FS_DEFINE_CPO(is_other);
EXPECTED_FS_DEFINE_CPO(is_regular_file);
EXPECTED_FS_DEFINE_CPO(is_socket);
EXPECTED_FS_DEFINE_CPO(is_symlink);
EXPECTED_FS_DEFINE_CPO(file_size);
EXPECTED_FS_DEFINE_CPO(hard_link_count);
EXPECTED_FS_DEFINE_CPO(last_write_time);
EXPECTED_FS_DEFINE_CPO(space);
EXPECTED_FS_DEFINE_CPO(copy);
EXPECTED_FS_DEFINE_CPO(copy_file);
EXPECTED_FS_DEFINE_CPO(copy_symlink);
EXPECTED_FS_DEFINE_CPO(create_directory);
EXPECTED_FS_DEFINE_CPO(create_directories);
EXPECTED_FS_DEFINE_CPO(create_hard_link);
EXPECTED_FS_DEFINE_CPO(create_symlink);
EXPECTED_FS_DEFINE_CPO(create_directory_symlink);
EXPECTED_FS_DEFINE_CPO(permissions);
EXPECTED_FS_DEFINE_CPO(remove);
EXPECTED_FS_DEFINE_CPO(remove_all);
EXPECTED_FS_DEFINE_CPO(rename);
EXPECTED_FS_DEFINE_CPO(resize_file);
EXPECTED_FS_DEFINE_CPO(make_directory_entry);
EXPECTED_FS_DEFINE_CPO(assign);
EXPECTED_FS_DEFINE_CPO(replace_filename);
EXPECTED_FS_DEFINE_CPO(refresh);
EXPECTED_FS_DEFINE_CPO(make_directory_iterator);
EXPECTED_FS_DEFINE_CPO(increment);
EXPECTED_FS_DEFINE_CPO(make_recursive_directory_iterator);
EXPECTED_FS_DEFINE_CPO(pop);
EXPECTED_FS_DEFINE_CPO(u8path);
EXPECTED_FS_DEFINE_ACTION_CPO(swap);
EXPECTED_FS_DEFINE_CPO(hash_value);
EXPECTED_FS_DEFINE_CPO(status_known);
EXPECTED_FS_DEFINE_CPO(begin);
EXPECTED_FS_DEFINE_CPO(end);
EXPECTED_FS_DEFINE_CPO(options);
EXPECTED_FS_DEFINE_CPO(depth);
EXPECTED_FS_DEFINE_CPO(recursion_pending);
EXPECTED_FS_DEFINE_ACTION_CPO(disable_recursion_pending);

#undef EXPECTED_FS_DEFINE_ACTION_CPO
#undef EXPECTED_FS_DEFINE_CPO

template <class Fs, class Source>
  requires detail::filesystem_traits<Fs> && requires(Fs &fs, Source &&source) {
    {
      fs_ops_t<Fs>::u8path(fs, std::forward<Source>(source))
    } -> std::convertible_to<fs_path_t<Fs>>;
  }
[[nodiscard]] inline auto
raw_ops_invoke(u8path_t, Fs &fs, Source &&source) noexcept(
    noexcept(static_cast<fs_path_t<Fs>>(fs_ops_t<Fs>::u8path(
        fs, std::forward<Source>(source))))) -> fs_path_t<Fs> {
  return static_cast<fs_path_t<Fs>>(
      fs_ops_t<Fs>::u8path(fs, std::forward<Source>(source)));
}

template <class Fs, class InputIterator>
  requires detail::filesystem_traits<Fs>
        && requires(Fs &fs, InputIterator first, InputIterator last) {
             {
               fs_ops_t<Fs>::u8path(fs, first, last)
             } -> std::convertible_to<fs_path_t<Fs>>;
           }
[[nodiscard]] inline auto raw_ops_invoke(
    u8path_t, Fs &fs, InputIterator first,
    InputIterator
        last) noexcept(noexcept(static_cast<fs_path_t<Fs>>(fs_ops_t<Fs>::
                                                               u8path(fs, first,
                                                                      last))))
    -> fs_path_t<Fs> {
  return static_cast<fs_path_t<Fs>>(fs_ops_t<Fs>::u8path(fs, first, last));
}

template <class Fs>
  requires detail::filesystem_traits<Fs>
        && requires(Fs &fs, fs_path_t<Fs> &left, fs_path_t<Fs> &right) {
             { fs_ops_t<Fs>::swap(fs, left, right) } -> std::same_as<void>;
           }
inline void raw_ops_invoke(
    swap_t, Fs &fs, fs_path_t<Fs> &left,
    fs_path_t<Fs> &right) noexcept(noexcept(fs_ops_t<Fs>::swap(fs, left,
                                                               right))) {
  fs_ops_t<Fs>::swap(fs, left, right);
}

template <class Fs>
  requires detail::filesystem_traits<Fs>
        && requires(Fs &fs, const fs_path_t<Fs> &value) {
             {
               fs_ops_t<Fs>::hash_value(fs, value)
             } -> std::convertible_to<std::size_t>;
           }
[[nodiscard]] inline auto
raw_ops_invoke(hash_value_t, Fs &fs, const fs_path_t<Fs> &value) noexcept(
    noexcept(static_cast<std::size_t>(fs_ops_t<Fs>::hash_value(fs, value))))
    -> std::size_t {
  return static_cast<std::size_t>(fs_ops_t<Fs>::hash_value(fs, value));
}

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define EXPECTED_FS_RAW_STATUS_OPERATION(cpo, operation)                       \
  template <class Fs, class Status>                                            \
    requires detail::filesystem_file_status<Fs>                                \
          && std::same_as<std::remove_cvref_t<Status>, fs_file_status_t<Fs>>   \
          && requires(Fs &fs, Status value) {                                  \
               {                                                               \
                 fs_ops_t<Fs>::operation(fs, value)                            \
               } -> std::convertible_to<bool>;                                 \
             }                                                                 \
  [[nodiscard]] inline bool                                                    \
  raw_ops_invoke(cpo##_t, Fs &fs, Status value) noexcept(                      \
      noexcept(static_cast<bool>(fs_ops_t<Fs>::operation(fs, value)))) {       \
    return static_cast<bool>(fs_ops_t<Fs>::operation(fs, value));              \
  }

EXPECTED_FS_RAW_STATUS_OPERATION(exists, exists)
EXPECTED_FS_RAW_STATUS_OPERATION(is_block_file, is_block_file)
EXPECTED_FS_RAW_STATUS_OPERATION(is_character_file, is_character_file)
EXPECTED_FS_RAW_STATUS_OPERATION(is_directory, is_directory)
EXPECTED_FS_RAW_STATUS_OPERATION(is_fifo, is_fifo)
EXPECTED_FS_RAW_STATUS_OPERATION(is_other, is_other)
EXPECTED_FS_RAW_STATUS_OPERATION(is_regular_file, is_regular_file)
EXPECTED_FS_RAW_STATUS_OPERATION(is_socket, is_socket)
EXPECTED_FS_RAW_STATUS_OPERATION(is_symlink, is_symlink)

#undef EXPECTED_FS_RAW_STATUS_OPERATION
// NOLINTEND(cppcoreguidelines-macro-usage)

template <class Fs>
  requires detail::filesystem_file_status<Fs>
        && requires(Fs &fs, fs_file_status_t<Fs> value) {
             {
               fs_ops_t<Fs>::status_known(fs, value)
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline bool
raw_ops_invoke(status_known_t, Fs &fs, fs_file_status_t<Fs> value) noexcept(
    noexcept(static_cast<bool>(fs_ops_t<Fs>::status_known(fs, value)))) {
  return static_cast<bool>(fs_ops_t<Fs>::status_known(fs, value));
}

template <class Fs>
  requires detail::filesystem_directory_iterator<Fs>
        && requires(Fs &fs, fs_directory_iterator_t<Fs> iterator) {
             {
               fs_ops_t<Fs>::begin(fs, std::move(iterator))
             } -> std::convertible_to<fs_directory_iterator_t<Fs>>;
           }
[[nodiscard]] inline auto
raw_ops_invoke(begin_t, Fs &fs, fs_directory_iterator_t<Fs> iterator) noexcept(
    noexcept(static_cast<fs_directory_iterator_t<Fs>>(fs_ops_t<Fs>::begin(
        fs, std::move(iterator))))) -> fs_directory_iterator_t<Fs> {
  return static_cast<fs_directory_iterator_t<Fs>>(
      fs_ops_t<Fs>::begin(fs, std::move(iterator)));
}

template <class Fs>
  requires detail::filesystem_recursive_directory_iterator<Fs>
        && (!detail::filesystem_directory_iterator<Fs>
            || (!std::same_as<fs_directory_iterator_t<Fs>,
                              fs_recursive_directory_iterator_t<Fs>>))
        && requires(Fs &fs, fs_recursive_directory_iterator_t<Fs> iterator) {
             {
               fs_ops_t<Fs>::begin(fs, std::move(iterator))
             } -> std::convertible_to<fs_recursive_directory_iterator_t<Fs>>;
           }
[[nodiscard]] inline auto
raw_ops_invoke(begin_t, Fs &fs, fs_recursive_directory_iterator_t<Fs> iterator) noexcept(
    noexcept(static_cast<fs_recursive_directory_iterator_t<Fs>>(
        fs_ops_t<Fs>::begin(fs, std::move(iterator)))))
    -> fs_recursive_directory_iterator_t<Fs> {
  return static_cast<fs_recursive_directory_iterator_t<Fs>>(
      fs_ops_t<Fs>::begin(fs, std::move(iterator)));
}

template <class Fs>
  requires detail::filesystem_directory_iterator<Fs>
        && requires(Fs &fs, fs_directory_iterator_t<Fs> iterator) {
             {
               fs_ops_t<Fs>::end(fs, std::move(iterator))
             } -> std::convertible_to<fs_directory_iterator_t<Fs>>;
           }
[[nodiscard]] inline auto
raw_ops_invoke(end_t, Fs &fs, fs_directory_iterator_t<Fs> iterator) noexcept(
    noexcept(static_cast<fs_directory_iterator_t<Fs>>(fs_ops_t<Fs>::end(
        fs, std::move(iterator))))) -> fs_directory_iterator_t<Fs> {
  return static_cast<fs_directory_iterator_t<Fs>>(
      fs_ops_t<Fs>::end(fs, std::move(iterator)));
}

template <class Fs>
  requires detail::filesystem_recursive_directory_iterator<Fs>
        && (!detail::filesystem_directory_iterator<Fs>
            || (!std::same_as<fs_directory_iterator_t<Fs>,
                              fs_recursive_directory_iterator_t<Fs>>))
        && requires(Fs &fs, fs_recursive_directory_iterator_t<Fs> iterator) {
             {
               fs_ops_t<Fs>::end(fs, std::move(iterator))
             } -> std::convertible_to<fs_recursive_directory_iterator_t<Fs>>;
           }
[[nodiscard]] inline auto
raw_ops_invoke(end_t, Fs &fs, fs_recursive_directory_iterator_t<Fs> iterator) noexcept(
    noexcept(static_cast<fs_recursive_directory_iterator_t<Fs>>(
        fs_ops_t<Fs>::end(fs, std::move(iterator)))))
    -> fs_recursive_directory_iterator_t<Fs> {
  return static_cast<fs_recursive_directory_iterator_t<Fs>>(
      fs_ops_t<Fs>::end(fs, std::move(iterator)));
}

template <class Fs>
  requires detail::filesystem_recursive_directory_iterator<Fs>
        && requires(Fs &fs,
                    const fs_recursive_directory_iterator_t<Fs> &iterator) {
             {
               fs_ops_t<Fs>::options(fs, iterator)
             } -> std::convertible_to<fs_directory_options_t<Fs>>;
           }
[[nodiscard]] inline auto
raw_ops_invoke(options_t, Fs &fs, const fs_recursive_directory_iterator_t<Fs> &iterator) noexcept(
    noexcept(static_cast<fs_directory_options_t<Fs>>(
        fs_ops_t<Fs>::options(fs, iterator)))) -> fs_directory_options_t<Fs> {
  return static_cast<fs_directory_options_t<Fs>>(
      fs_ops_t<Fs>::options(fs, iterator));
}

template <class Fs>
  requires detail::filesystem_recursive_directory_iterator<Fs>
        && requires(Fs &fs,
                    const fs_recursive_directory_iterator_t<Fs> &iterator) {
             { fs_ops_t<Fs>::depth(fs, iterator) } -> std::convertible_to<int>;
           }
[[nodiscard]] inline int raw_ops_invoke(
    depth_t, Fs &fs,
    const fs_recursive_directory_iterator_t<Fs>
        &iterator) noexcept(noexcept(static_cast<int>(fs_ops_t<Fs>::
                                                          depth(fs,
                                                                iterator)))) {
  return static_cast<int>(fs_ops_t<Fs>::depth(fs, iterator));
}

template <class Fs>
  requires detail::filesystem_recursive_directory_iterator<Fs>
        && requires(Fs &fs,
                    const fs_recursive_directory_iterator_t<Fs> &iterator) {
             {
               fs_ops_t<Fs>::recursion_pending(fs, iterator)
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline bool raw_ops_invoke(
    recursion_pending_t, Fs &fs,
    const fs_recursive_directory_iterator_t<Fs> &
        iterator) noexcept(noexcept(static_cast<bool>(fs_ops_t<Fs>::
                                                          recursion_pending(
                                                              fs, iterator)))) {
  return static_cast<bool>(fs_ops_t<Fs>::recursion_pending(fs, iterator));
}

template <class Fs>
  requires detail::filesystem_recursive_directory_iterator<Fs>
        && requires(Fs &fs, fs_recursive_directory_iterator_t<Fs> &iterator) {
             {
               fs_ops_t<Fs>::disable_recursion_pending(fs, iterator)
             } -> std::same_as<void>;
           }
inline void
raw_ops_invoke(disable_recursion_pending_t, Fs &fs,
               fs_recursive_directory_iterator_t<Fs>
                   &iterator) noexcept(noexcept(fs_ops_t<Fs>::
                                                    disable_recursion_pending(
                                                        fs, iterator))) {
  fs_ops_t<Fs>::disable_recursion_pending(fs, iterator);
}

// Low-priority operations fallback. The CPO dispatcher reaches these overloads
// only when no direct or policy-aware tag_invoke customization is available.
// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define EXPECTED_FS_OPS_VALUE_0(cpo, operation, result_type)                   \
  template <class Adaptor>                                                     \
    requires detail::result_adaptor_instance<Adaptor>                          \
          && requires(Adaptor &adaptor, std::error_code &error) {              \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(           \
                     adaptor.fs, error)                                        \
               } -> std::convertible_to<result_type>;                          \
             }                                                                 \
  [[nodiscard]] inline auto ops_invoke(cpo##_t, Adaptor &adaptor) {            \
    using policy = detail::adaptor_policy_t<Adaptor>;                          \
    std::error_code error;                                                     \
    auto result = static_cast<result_type>(                                    \
        fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(adaptor.fs,         \
                                                           error));            \
    return detail::make_result<policy>(std::move(result), error);              \
  }

#define EXPECTED_FS_OPS_VALUE_1(cpo, operation, result_type, argument_type)    \
  template <class Adaptor>                                                     \
    requires detail::result_adaptor_instance<Adaptor>                          \
          && requires(Adaptor &adaptor, const argument_type &value,            \
                      std::error_code &error) {                                \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(           \
                     adaptor.fs, value, error)                                 \
               } -> std::convertible_to<result_type>;                          \
             }                                                                 \
  [[nodiscard]] inline auto ops_invoke(cpo##_t, Adaptor &adaptor,              \
                                       const argument_type &value) {           \
    using policy = detail::adaptor_policy_t<Adaptor>;                          \
    std::error_code error;                                                     \
    auto result = static_cast<result_type>(                                    \
        fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(adaptor.fs, value,  \
                                                           error));            \
    return detail::make_result<policy>(std::move(result), error);              \
  }

#define EXPECTED_FS_OPS_VALUE_2(cpo, operation, result_type, argument1_type,   \
                                argument2_type)                                \
  template <class Adaptor>                                                     \
    requires detail::result_adaptor_instance<Adaptor>                          \
          && requires(Adaptor &adaptor, const argument1_type &argument1,       \
                      const argument2_type &argument2,                         \
                      std::error_code &error) {                                \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(           \
                     adaptor.fs, argument1, argument2, error)                  \
               } -> std::convertible_to<result_type>;                          \
             }                                                                 \
  [[nodiscard]] inline auto ops_invoke(cpo##_t, Adaptor &adaptor,              \
                                       const argument1_type &argument1,        \
                                       const argument2_type &argument2) {      \
    using policy = detail::adaptor_policy_t<Adaptor>;                          \
    std::error_code error;                                                     \
    auto result = static_cast<result_type>(                                    \
        fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(                    \
            adaptor.fs, argument1, argument2, error));                         \
    return detail::make_result<policy>(std::move(result), error);              \
  }

#define EXPECTED_FS_OPS_VOID_1(cpo, operation, argument_type)                  \
  template <class Adaptor>                                                     \
    requires detail::result_adaptor_instance<Adaptor>                          \
          && requires(Adaptor &adaptor, const argument_type &value,            \
                      std::error_code &error) {                                \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(           \
                     adaptor.fs, value, error)                                 \
               } -> std::same_as<void>;                                        \
             }                                                                 \
  [[nodiscard]] inline auto ops_invoke(cpo##_t, Adaptor &adaptor,              \
                                       const argument_type &value) {           \
    using policy = detail::adaptor_policy_t<Adaptor>;                          \
    std::error_code error;                                                     \
    fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(adaptor.fs, value,      \
                                                       error);                 \
    return detail::make_result<policy>(error);                                 \
  }

#define EXPECTED_FS_OPS_VOID_2(cpo, operation, argument1_type, argument2_type) \
  template <class Adaptor>                                                     \
    requires detail::result_adaptor_instance<Adaptor>                          \
          && requires(Adaptor &adaptor, const argument1_type &argument1,       \
                      const argument2_type &argument2,                         \
                      std::error_code &error) {                                \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(           \
                     adaptor.fs, argument1, argument2, error)                  \
               } -> std::same_as<void>;                                        \
             }                                                                 \
  [[nodiscard]] inline auto ops_invoke(cpo##_t, Adaptor &adaptor,              \
                                       const argument1_type &argument1,        \
                                       const argument2_type &argument2) {      \
    using policy = detail::adaptor_policy_t<Adaptor>;                          \
    std::error_code error;                                                     \
    fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(adaptor.fs, argument1,  \
                                                       argument2, error);      \
    return detail::make_result<policy>(error);                                 \
  }

EXPECTED_FS_OPS_VALUE_1(absolute, absolute,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_1(canonical, canonical,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_1(weakly_canonical, weakly_canonical,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_1(relative, relative,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_2(relative, relative,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_1(proximate, proximate,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_2(proximate, proximate,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_1(read_symlink, read_symlink,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_0(temp_directory_path, temp_directory_path,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VALUE_0(current_path, current_path,
                        fs_path_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_VOID_1(current_path, current_path,
                       fs_path_t<detail::adaptor_fs_t<Adaptor>>)

#undef EXPECTED_FS_OPS_VALUE_0
#undef EXPECTED_FS_OPS_VALUE_1
#undef EXPECTED_FS_OPS_VALUE_2
#undef EXPECTED_FS_OPS_VOID_1
#undef EXPECTED_FS_OPS_VOID_2
// NOLINTEND(cppcoreguidelines-macro-usage)

template <class Adaptor, class Source>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor, const Source &source,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::status(adaptor.fs,
                                                               source, error)
             } -> std::convertible_to<
                 fs_file_status_t<detail::adaptor_fs_t<Adaptor>>>;
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::status_known(
                   adaptor.fs,
                   std::declval<
                       fs_file_status_t<detail::adaptor_fs_t<Adaptor>>>())
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline auto
ops_invoke(status_t, Adaptor &adaptor, const Source &source) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  using status_type = fs_file_status_t<fs_type>;
  std::error_code error;
  auto result = static_cast<status_type>(
      fs_ops_t<fs_type>::status(adaptor.fs, source, error));
  if (static_cast<bool>(fs_ops_t<fs_type>::status_known(adaptor.fs, result))) {
    return policy::template success<status_type>(std::move(result));
  }
  return detail::make_result<policy>(std::move(result), error);
}

template <class Adaptor, class Source>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor, const Source &source,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::symlink_status(
                   adaptor.fs, source, error)
             } -> std::convertible_to<
                 fs_file_status_t<detail::adaptor_fs_t<Adaptor>>>;
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::status_known(
                   adaptor.fs,
                   std::declval<
                       fs_file_status_t<detail::adaptor_fs_t<Adaptor>>>())
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline auto
ops_invoke(symlink_status_t, Adaptor &adaptor, const Source &source) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  using status_type = fs_file_status_t<fs_type>;
  std::error_code error;
  auto result = static_cast<status_type>(
      fs_ops_t<fs_type>::symlink_status(adaptor.fs, source, error));
  if (static_cast<bool>(fs_ops_t<fs_type>::status_known(adaptor.fs, result))) {
    return policy::template success<status_type>(std::move(result));
  }
  return detail::make_result<policy>(std::move(result), error);
}

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define EXPECTED_FS_OPS_STATUS_QUERY(cpo, query, predicate)                    \
  template <class Adaptor, class Source>                                       \
    requires detail::result_adaptor_instance<Adaptor>                          \
          && (!std::same_as<std::remove_cvref_t<Source>,                       \
                            fs_file_status_t<detail::adaptor_fs_t<Adaptor>>>)  \
          && requires(Adaptor &adaptor, const Source &source,                  \
                      std::error_code &error) {                                \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::query(adaptor.fs,    \
                                                                source, error) \
               } -> std::convertible_to<                                       \
                   fs_file_status_t<detail::adaptor_fs_t<Adaptor>>>;           \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::status_known(        \
                     adaptor.fs,                                               \
                     std::declval<                                             \
                         fs_file_status_t<detail::adaptor_fs_t<Adaptor>>>())   \
               } -> std::convertible_to<bool>;                                 \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::predicate(           \
                     adaptor.fs,                                               \
                     std::declval<                                             \
                         fs_file_status_t<detail::adaptor_fs_t<Adaptor>>>())   \
               } -> std::convertible_to<bool>;                                 \
             }                                                                 \
  [[nodiscard]] inline auto ops_invoke(cpo##_t, Adaptor &adaptor,              \
                                       const Source &source) {                 \
    using fs_type = detail::adaptor_fs_t<Adaptor>;                             \
    using policy = detail::adaptor_policy_t<Adaptor>;                          \
    using status_type = fs_file_status_t<fs_type>;                             \
    std::error_code error;                                                     \
    auto result = static_cast<status_type>(                                    \
        fs_ops_t<fs_type>::query(adaptor.fs, source, error));                  \
    if (!static_cast<bool>(                                                    \
            fs_ops_t<fs_type>::status_known(adaptor.fs, result))               \
        && error) {                                                            \
      return policy::template failure<bool>(error);                            \
    }                                                                          \
    return policy::template success<bool>(                                     \
        static_cast<bool>(fs_ops_t<fs_type>::predicate(adaptor.fs, result)));  \
  }

EXPECTED_FS_OPS_STATUS_QUERY(exists, status, exists)
EXPECTED_FS_OPS_STATUS_QUERY(is_block_file, status, is_block_file)
EXPECTED_FS_OPS_STATUS_QUERY(is_character_file, status, is_character_file)
EXPECTED_FS_OPS_STATUS_QUERY(is_directory, status, is_directory)
EXPECTED_FS_OPS_STATUS_QUERY(is_fifo, status, is_fifo)
EXPECTED_FS_OPS_STATUS_QUERY(is_other, status, is_other)
EXPECTED_FS_OPS_STATUS_QUERY(is_regular_file, status, is_regular_file)
EXPECTED_FS_OPS_STATUS_QUERY(is_socket, status, is_socket)
EXPECTED_FS_OPS_STATUS_QUERY(is_symlink, symlink_status, is_symlink)

#undef EXPECTED_FS_OPS_STATUS_QUERY
// NOLINTEND(cppcoreguidelines-macro-usage)

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &left,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &right,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::equivalent(
                   adaptor.fs, left, right, error)
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline auto
ops_invoke(equivalent_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &left,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &right) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  const bool result = static_cast<bool>(
      fs_ops_t<fs_type>::equivalent(adaptor.fs, left, right, error));
  return detail::make_result<policy>(result, error);
}

template <class Adaptor, class Source>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor, const Source &source,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::is_empty(adaptor.fs,
                                                                 source, error)
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline auto
ops_invoke(is_empty_t, Adaptor &adaptor, const Source &source) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  const bool result =
      static_cast<bool>(fs_ops_t<fs_type>::is_empty(adaptor.fs, source, error));
  return detail::make_result<policy>(result, error);
}

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define EXPECTED_FS_OPS_SOURCE_VALUE(cpo, operation, result_type)              \
  template <class Adaptor, class Source>                                       \
    requires detail::result_adaptor_instance<Adaptor>                          \
          && requires(Adaptor &adaptor, const Source &source,                  \
                      std::error_code &error) {                                \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(           \
                     adaptor.fs, source, error)                                \
               } -> std::convertible_to<result_type>;                          \
             }                                                                 \
  [[nodiscard]] inline auto ops_invoke(cpo##_t, Adaptor &adaptor,              \
                                       const Source &source) {                 \
    using fs_type = detail::adaptor_fs_t<Adaptor>;                             \
    using policy = detail::adaptor_policy_t<Adaptor>;                          \
    std::error_code error;                                                     \
    auto result = static_cast<result_type>(                                    \
        fs_ops_t<fs_type>::operation(adaptor.fs, source, error));              \
    return detail::make_result<policy>(std::move(result), error);              \
  }

EXPECTED_FS_OPS_SOURCE_VALUE(file_size, file_size, std::uintmax_t)
EXPECTED_FS_OPS_SOURCE_VALUE(hard_link_count, hard_link_count, std::uintmax_t)
EXPECTED_FS_OPS_SOURCE_VALUE(last_write_time, last_write_time,
                             fs_file_time_type_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_SOURCE_VALUE(space, space,
                             fs_space_info_t<detail::adaptor_fs_t<Adaptor>>)
EXPECTED_FS_OPS_SOURCE_VALUE(create_directory, create_directory, bool)
EXPECTED_FS_OPS_SOURCE_VALUE(create_directories, create_directories, bool)
EXPECTED_FS_OPS_SOURCE_VALUE(remove, remove, bool)
EXPECTED_FS_OPS_SOURCE_VALUE(remove_all, remove_all, std::uintmax_t)

#undef EXPECTED_FS_OPS_SOURCE_VALUE
// NOLINTEND(cppcoreguidelines-macro-usage)

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    fs_file_time_type_t<detail::adaptor_fs_t<Adaptor>> new_time,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::last_write_time(
                   adaptor.fs, value, new_time, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(last_write_time_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
           fs_file_time_type_t<detail::adaptor_fs_t<Adaptor>> new_time) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::last_write_time(adaptor.fs, value, new_time, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(
               Adaptor &adaptor,
               const fs_path_t<detail::adaptor_fs_t<Adaptor>> &from,
               const fs_path_t<detail::adaptor_fs_t<Adaptor>> &to,
               fs_copy_options_t<detail::adaptor_fs_t<Adaptor>> option_value,
               std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::copy(
                   adaptor.fs, from, to, option_value, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(copy_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &from,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &to,
           fs_copy_options_t<detail::adaptor_fs_t<Adaptor>> option_value) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::copy(adaptor.fs, from, to, option_value, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &from,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &to,
                    std::error_code &error) {
             fs_copy_options_t<detail::adaptor_fs_t<Adaptor>>::none;
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::copy(
                   adaptor.fs, from, to,
                   fs_copy_options_t<detail::adaptor_fs_t<Adaptor>>::none,
                   error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(copy_t tag, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &from,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &to) {
  return tag(adaptor, from, to,
             fs_copy_options_t<detail::adaptor_fs_t<Adaptor>>::none);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(
               Adaptor &adaptor,
               const fs_path_t<detail::adaptor_fs_t<Adaptor>> &from,
               const fs_path_t<detail::adaptor_fs_t<Adaptor>> &to,
               fs_copy_options_t<detail::adaptor_fs_t<Adaptor>> option_value,
               std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::copy_file(
                   adaptor.fs, from, to, option_value, error)
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline auto
ops_invoke(copy_file_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &from,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &to,
           fs_copy_options_t<detail::adaptor_fs_t<Adaptor>> option_value) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  const bool result = static_cast<bool>(
      fs_ops_t<fs_type>::copy_file(adaptor.fs, from, to, option_value, error));
  return detail::make_result<policy>(result, error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &from,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &to,
                    std::error_code &error) {
             fs_copy_options_t<detail::adaptor_fs_t<Adaptor>>::none;
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::copy_file(
                   adaptor.fs, from, to,
                   fs_copy_options_t<detail::adaptor_fs_t<Adaptor>>::none,
                   error)
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline auto
ops_invoke(copy_file_t tag, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &from,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &to) {
  return tag(adaptor, from, to,
             fs_copy_options_t<detail::adaptor_fs_t<Adaptor>>::none);
}

// NOLINTBEGIN(cppcoreguidelines-macro-usage)
#define EXPECTED_FS_OPS_PATHS_VOID(cpo, operation)                             \
  template <class Adaptor>                                                     \
    requires detail::result_adaptor_instance<Adaptor>                          \
          && requires(Adaptor &adaptor,                                        \
                      const fs_path_t<detail::adaptor_fs_t<Adaptor>> &first,   \
                      const fs_path_t<detail::adaptor_fs_t<Adaptor>> &second,  \
                      std::error_code &error) {                                \
               {                                                               \
                 fs_ops_t<detail::adaptor_fs_t<Adaptor>>::operation(           \
                     adaptor.fs, first, second, error)                         \
               } -> std::same_as<void>;                                        \
             }                                                                 \
  [[nodiscard]] inline auto ops_invoke(                                        \
      cpo##_t, Adaptor &adaptor,                                               \
      const fs_path_t<detail::adaptor_fs_t<Adaptor>> &first,                   \
      const fs_path_t<detail::adaptor_fs_t<Adaptor>> &second) {                \
    using fs_type = detail::adaptor_fs_t<Adaptor>;                             \
    using policy = detail::adaptor_policy_t<Adaptor>;                          \
    std::error_code error;                                                     \
    fs_ops_t<fs_type>::operation(adaptor.fs, first, second, error);            \
    return detail::make_result<policy>(error);                                 \
  }

EXPECTED_FS_OPS_PATHS_VOID(copy_symlink, copy_symlink)
EXPECTED_FS_OPS_PATHS_VOID(create_hard_link, create_hard_link)
EXPECTED_FS_OPS_PATHS_VOID(create_symlink, create_symlink)
EXPECTED_FS_OPS_PATHS_VOID(create_directory_symlink, create_directory_symlink)
EXPECTED_FS_OPS_PATHS_VOID(rename, rename)

#undef EXPECTED_FS_OPS_PATHS_VOID
// NOLINTEND(cppcoreguidelines-macro-usage)

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &attributes,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::create_directory(
                   adaptor.fs, value, attributes, error)
             } -> std::convertible_to<bool>;
           }
[[nodiscard]] inline auto
ops_invoke(create_directory_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &attributes) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  const bool result = static_cast<bool>(fs_ops_t<fs_type>::create_directory(
      adaptor.fs, value, attributes, error));
  return detail::make_result<policy>(result, error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(
               Adaptor &adaptor,
               const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
               fs_perms_t<detail::adaptor_fs_t<Adaptor>> new_permissions,
               fs_perm_options_t<detail::adaptor_fs_t<Adaptor>> option_value,
               std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::permissions(
                   adaptor.fs, value, new_permissions, option_value, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(permissions_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
           fs_perms_t<detail::adaptor_fs_t<Adaptor>> new_permissions,
           fs_perm_options_t<detail::adaptor_fs_t<Adaptor>> option_value) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::permissions(adaptor.fs, value, new_permissions,
                                 option_value, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    fs_perms_t<detail::adaptor_fs_t<Adaptor>> new_permissions,
                    std::error_code &error) {
             fs_perm_options_t<detail::adaptor_fs_t<Adaptor>>::replace;
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::permissions(
                   adaptor.fs, value, new_permissions,
                   fs_perm_options_t<detail::adaptor_fs_t<Adaptor>>::replace,
                   error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(permissions_t tag, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
           fs_perms_t<detail::adaptor_fs_t<Adaptor>> new_permissions) {
  return tag(adaptor, value, new_permissions,
             fs_perm_options_t<detail::adaptor_fs_t<Adaptor>>::replace);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    std::uintmax_t size, std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::resize_file(
                   adaptor.fs, value, size, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(resize_file_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
           std::uintmax_t size) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::resize_file(adaptor.fs, value, size, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::make_directory_entry(
                   adaptor.fs, value, error)
             } -> std::convertible_to<
                 fs_directory_entry_t<detail::adaptor_fs_t<Adaptor>>>;
           }
[[nodiscard]] inline auto
ops_invoke(make_directory_entry_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  using entry_type = fs_directory_entry_t<fs_type>;
  std::error_code error;
  auto result = static_cast<entry_type>(
      fs_ops_t<fs_type>::make_directory_entry(adaptor.fs, value, error));
  return detail::make_result<policy>(std::move(result), error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    fs_directory_entry_t<detail::adaptor_fs_t<Adaptor>> &entry,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::assign(
                   adaptor.fs, entry, value, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(assign_t, Adaptor &adaptor,
           fs_directory_entry_t<detail::adaptor_fs_t<Adaptor>> &entry,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::assign(adaptor.fs, entry, value, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    fs_directory_entry_t<detail::adaptor_fs_t<Adaptor>> &entry,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::replace_filename(
                   adaptor.fs, entry, value, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(replace_filename_t, Adaptor &adaptor,
           fs_directory_entry_t<detail::adaptor_fs_t<Adaptor>> &entry,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::replace_filename(adaptor.fs, entry, value, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    fs_directory_entry_t<detail::adaptor_fs_t<Adaptor>> &entry,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::refresh(adaptor.fs,
                                                                entry, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(refresh_t, Adaptor &adaptor,
           fs_directory_entry_t<detail::adaptor_fs_t<Adaptor>> &entry) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::refresh(adaptor.fs, entry, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    fs_directory_options_t<detail::adaptor_fs_t<Adaptor>>
                        option_value,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::make_directory_iterator(
                   adaptor.fs, value, option_value, error)
             } -> std::convertible_to<
                 fs_directory_iterator_t<detail::adaptor_fs_t<Adaptor>>>;
           }
[[nodiscard]] inline auto
ops_invoke(make_directory_iterator_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
           fs_directory_options_t<detail::adaptor_fs_t<Adaptor>> option_value) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  using iterator_type = fs_directory_iterator_t<fs_type>;
  std::error_code error;
  auto result =
      static_cast<iterator_type>(fs_ops_t<fs_type>::make_directory_iterator(
          adaptor.fs, value, option_value, error));
  return detail::make_result<policy>(std::move(result), error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    std::error_code &error) {
             fs_directory_options_t<detail::adaptor_fs_t<Adaptor>>::none;
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::make_directory_iterator(
                   adaptor.fs, value,
                   fs_directory_options_t<detail::adaptor_fs_t<Adaptor>>::none,
                   error)
             } -> std::convertible_to<
                 fs_directory_iterator_t<detail::adaptor_fs_t<Adaptor>>>;
           }
[[nodiscard]] inline auto
ops_invoke(make_directory_iterator_t tag, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value) {
  return tag(adaptor, value,
             fs_directory_options_t<detail::adaptor_fs_t<Adaptor>>::none);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(
               Adaptor &adaptor,
               fs_directory_iterator_t<detail::adaptor_fs_t<Adaptor>> &iterator,
               std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::increment(
                   adaptor.fs, iterator, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(increment_t, Adaptor &adaptor,
           fs_directory_iterator_t<detail::adaptor_fs_t<Adaptor>> &iterator) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::increment(adaptor.fs, iterator, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    fs_directory_options_t<detail::adaptor_fs_t<Adaptor>>
                        option_value,
                    std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::
                   make_recursive_directory_iterator(adaptor.fs, value,
                                                     option_value, error)
             } -> std::convertible_to<fs_recursive_directory_iterator_t<
                 detail::adaptor_fs_t<Adaptor>>>;
           }
[[nodiscard]] inline auto
ops_invoke(make_recursive_directory_iterator_t, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
           fs_directory_options_t<detail::adaptor_fs_t<Adaptor>> option_value) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  using iterator_type = fs_recursive_directory_iterator_t<fs_type>;
  std::error_code error;
  auto result = static_cast<iterator_type>(
      fs_ops_t<fs_type>::make_recursive_directory_iterator(
          adaptor.fs, value, option_value, error));
  return detail::make_result<policy>(std::move(result), error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(Adaptor &adaptor,
                    const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value,
                    std::error_code &error) {
             fs_directory_options_t<detail::adaptor_fs_t<Adaptor>>::none;
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::
                   make_recursive_directory_iterator(
                       adaptor.fs, value,
                       fs_directory_options_t<
                           detail::adaptor_fs_t<Adaptor>>::none,
                       error)
             } -> std::convertible_to<fs_recursive_directory_iterator_t<
                 detail::adaptor_fs_t<Adaptor>>>;
           }
[[nodiscard]] inline auto
ops_invoke(make_recursive_directory_iterator_t tag, Adaptor &adaptor,
           const fs_path_t<detail::adaptor_fs_t<Adaptor>> &value) {
  return tag(adaptor, value,
             fs_directory_options_t<detail::adaptor_fs_t<Adaptor>>::none);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && (!std::same_as<
            fs_directory_iterator_t<detail::adaptor_fs_t<Adaptor>>,
            fs_recursive_directory_iterator_t<detail::adaptor_fs_t<Adaptor>>>)
        && requires(
               Adaptor &adaptor,
               fs_recursive_directory_iterator_t<detail::adaptor_fs_t<Adaptor>>
                   &iterator,
               std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::increment(
                   adaptor.fs, iterator, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(increment_t, Adaptor &adaptor,
           fs_recursive_directory_iterator_t<detail::adaptor_fs_t<Adaptor>>
               &iterator) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::increment(adaptor.fs, iterator, error);
  return detail::make_result<policy>(error);
}

template <class Adaptor>
  requires detail::result_adaptor_instance<Adaptor>
        && requires(
               Adaptor &adaptor,
               fs_recursive_directory_iterator_t<detail::adaptor_fs_t<Adaptor>>
                   &iterator,
               std::error_code &error) {
             {
               fs_ops_t<detail::adaptor_fs_t<Adaptor>>::pop(adaptor.fs,
                                                            iterator, error)
             } -> std::same_as<void>;
           }
[[nodiscard]] inline auto
ops_invoke(pop_t, Adaptor &adaptor,
           fs_recursive_directory_iterator_t<detail::adaptor_fs_t<Adaptor>>
               &iterator) {
  using fs_type = detail::adaptor_fs_t<Adaptor>;
  using policy = detail::adaptor_policy_t<Adaptor>;
  std::error_code error;
  fs_ops_t<fs_type>::pop(adaptor.fs, iterator, error);
  return detail::make_result<policy>(error);
}

} // namespace expected_fs

#endif

#endif // EXPECTED_FS_EXPECTED_FS_HPP_
