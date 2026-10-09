#pragma once 
#include "primitive_types.hpp"
#include "serial_format.hpp"
#include <string>
#include <format>
#include <concepts>


template<typename T>
struct ScalarSerializer{};

// NOTE: integer specialzation

template<typename T> 
inline constexpr auto serialize_scalar_value([[maybe_unused]] T v) -> std::string{
    static_assert(false, "Unable to serialize non-scalar type.");
    return "ERR_NON_SCALAR";
}
template<typename T> 
    requires std::integral<T> && (!std::same_as<T,bool>)
inline constexpr auto serialize_scalar_value(T v) -> std::string{
    return std::format("{}",v);
}

template<typename T> 
    requires std::floating_point<T>
inline constexpr auto serialize_scalar_value(T val) -> std::string{
    auto s = std::format("{}",val);
    if (std::ranges::find(s, '.') == s.end()){
        s+=".0";
    }
    return s;
}

template<>
inline constexpr auto serialize_scalar_value<std::string>(std::string val) -> std::string{
    return std::format("\'{}\'",val);
}

inline constexpr auto serialize_scalar_value(bool v) -> std::string{
    return std::string(v ? "true" : "false");
}
    
template<typename T>
concept is_serializable_scalar = 
    (std::integral<T> && (!std::same_as<T,bool>))
    || std::floating_point<T>
    || std::same_as<T,bool>
    || std::convertible_to<T, std::string>;


// if its a serializable scalar, we stop recursion
