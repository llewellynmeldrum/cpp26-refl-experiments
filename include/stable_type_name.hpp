#pragma once 
#include <concepts>

#include "primitive_types.hpp"
#include "cppslop.hpp"

template<typename T>
inline consteval auto stable_type_name(){
    #define DECL_STABLE_NAME(U, NAME) if (std::same_as<std::remove_cvref_t<T>,U>) {return NAME ##sv;}
    DECL_STABLE_NAME( i8, "i8")
    DECL_STABLE_NAME(i16, "i16")
    DECL_STABLE_NAME(i32, "i32")
    DECL_STABLE_NAME(i64, "i64")

    DECL_STABLE_NAME( u8, "u8")
    DECL_STABLE_NAME(u16, "u16")
    DECL_STABLE_NAME(u32, "u32")
    DECL_STABLE_NAME(u64, "u64")

    DECL_STABLE_NAME(bool, "bool")
    DECL_STABLE_NAME(f16, "f16")
    DECL_STABLE_NAME(f32, "f32")
    DECL_STABLE_NAME(f64, "f64")

    DECL_STABLE_NAME(std::string, "std::string")
    DECL_STABLE_NAME(std::string_view, "std::string_view")
    #undef DECL_STABLE_NAME
    return "unknown type name, try the meta:: alternative"sv;
}
