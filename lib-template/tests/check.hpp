#pragma once
// Minimal test harness: no dependency, no registration, no main() magic.
// A test file is a normal program; it returns the number of failed checks.
//
//     int main() {
//         CHECK(1 + 1 == 2);
//         CHECK_EQ(mylib::square(3), 9);
//         return check::result();
//     }
//
// Compile-time checks need no harness at all: just static_assert.
#include <cstdio>
#include <format>
#include <source_location>
#include <string>
#include <string_view>

namespace check {

inline int failures = 0;
inline int total = 0;

inline auto report(bool ok, std::string_view what, std::source_location loc) -> void {
    ++total;
    if (ok) return;
    ++failures;
    auto msg = std::format("\033[31mFAIL\033[0m {}:{}: {}\n", loc.file_name(), loc.line(), what);
    std::fputs(msg.c_str(), stderr);
}

template<typename T>
auto show(T const& v) -> std::string {
    if constexpr (requires { std::format("{}", v); }) {
        return std::format("{}", v);
    } else {
        return "<unformattable>";
    }
}

template<typename A, typename B>
auto equal(A const& a, B const& b, std::string_view a_expr, std::string_view b_expr,
           std::source_location loc = std::source_location::current()) -> void {
    bool ok = (a == b);
    report(ok, ok ? "" : std::format("{} == {}  ({} vs {})", a_expr, b_expr, show(a), show(b)), loc);
}

inline auto result() -> int {
    auto msg = std::format("{}/{} checks passed\n", total - failures, total);
    std::fputs(msg.c_str(), failures ? stderr : stdout);
    return failures;
}

} // namespace check

#define CHECK(expr)     ::check::report(static_cast<bool>(expr), #expr, std::source_location::current())
#define CHECK_EQ(a, b)  ::check::equal((a), (b), #a, #b)
