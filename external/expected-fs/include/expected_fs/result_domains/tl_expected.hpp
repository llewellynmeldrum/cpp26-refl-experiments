#ifndef EXPECTED_FS_RESULT_DOMAINS_TL_EXPECTED_HPP_
#define EXPECTED_FS_RESULT_DOMAINS_TL_EXPECTED_HPP_

#include <expected_fs/expected_fs.hpp> // IWYU pragma: export

#include <tl/expected.hpp> // IWYU pragma: export

#include <type_traits>
#include <utility>

namespace expected_fs {
struct tl_expected_result_domain {};
inline constexpr tl_expected_result_domain tl_expected_result{};

template <>
struct result_domain_traits<tl_expected_result_domain> {
  template <class T, class E>
  using result = tl::expected<T, E>;
};

template <class T, class E, class Value>
  requires(!std::is_void_v<T>)
[[nodiscard]] inline auto
tag_invoke(result_success_t, tl_expected_result_domain, std::type_identity<T>,
           std::type_identity<E>, Value &&value)
    -> result_t<tl_expected_result_domain, T, E> {
  return result_t<tl_expected_result_domain, T, E>{
      tl::in_place,
      std::forward<Value>(value),
  };
}

template <class E>
[[nodiscard]] inline auto
tag_invoke(result_success_t, tl_expected_result_domain,
           std::type_identity<void>, std::type_identity<E>)
    -> result_t<tl_expected_result_domain, void, E> {
  return {};
}

template <class T, class E, class Err>
[[nodiscard]] inline auto
tag_invoke(result_failure_t, tl_expected_result_domain, std::type_identity<T>,
           std::type_identity<E>, Err &&error)
    -> result_t<tl_expected_result_domain, T, E> {
  return result_t<tl_expected_result_domain, T, E>{
      tl::unexpect,
      E{std::forward<Err>(error)},
  };
}
} // namespace expected_fs

#endif // EXPECTED_FS_RESULT_DOMAINS_TL_EXPECTED_HPP_
