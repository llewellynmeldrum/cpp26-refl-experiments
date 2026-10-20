#pragma once 
#include <concepts>
#include <string_view>

#include "cfg_registration.hpp"
#include "concept_helpers.hpp"
#include "primitive_types.hpp"
#include "cppslop.hpp"

namespace cfg{

template<typename T>
concept valid_registration = requires(T v){
    sizeof(Registration<T>); // check if the specialization exists at all

    {Registration<T>::config_filename} -> same_as_nocvref<std::string_view>;
    {Registration<T>::config_name} -> same_as_nocvref<std::string_view>;
    {Registration<T>::description} -> same_as_nocvref<std::string_view>;

    requires Registration<T>::config_filename.ends_with(".toml");
};

template<typename T> requires cfg::valid_registration<T>
auto get_default() -> T { return T{}; }
} // namespace cfg
//
template<typename T>
inline consteval auto stable_type_name_primitives(){
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
    #undef DECL_STABLE_NAME
    return "undefined typename";
}
