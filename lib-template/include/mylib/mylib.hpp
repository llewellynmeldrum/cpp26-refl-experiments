#pragma once
#include <string>
#include <string_view>

// Placeholder API. Replace it with your library. It exists so every build step
// (tests, header check, install, consumer test) has something real to chew on.
namespace mylib {

[[nodiscard]] constexpr auto square(int x) -> int {
    return x * x;
}

[[nodiscard]] constexpr auto greeting(std::string_view who) -> std::string {
    return "hello, " + std::string(who);
}

} // namespace mylib
