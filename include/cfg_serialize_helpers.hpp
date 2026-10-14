#pragma once 
#include <string>
#include <format>
namespace cfg{
template<typename T>
inline constexpr auto serialize_toml_scalar_value(T const& val) -> std::string {
    if constexpr(std::floating_point<T>){
        auto s = std::format("{}",val);
        if (!s.contains(".")){
            return s + ".0";
        }
    } else if constexpr (std::integral<T> || std::same_as<bool, std::remove_cvref<T>>){
        return std::format("{}", val);
    }
}
} // NOTE: namespace cfg
