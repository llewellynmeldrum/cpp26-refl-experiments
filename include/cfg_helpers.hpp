#pragma once 
#include <concepts>
#include <string_view>
#include <meta>
#include <ranges>

#include "concept_helpers.hpp"
#include "primitive_types.hpp"
#include "cppslop.hpp"
#include "common.hpp"
#include "cfg_traits.hpp"
#include "cfg_metadata.hpp"
#include "meta_helpers.hpp"
#include "toml_traits.hpp"

#if CPP26
namespace cfg::meta{
inline consteval auto is_scalar_member(std::meta::info m){
    return cfg::meta::is_scalar_type(dealias(remove_cvref(type_of(m))));
}
inline consteval auto is_toml_array_member(std::meta::info m){
    return cfg::meta::is_array_type(dealias(remove_cvref(type_of(m))));
}
inline consteval auto is_struct_like_member(std::meta::info m){
    return cfg::meta::is_struct_like(dealias(remove_cvref(type_of(m))));
}
template<typename T>
inline constexpr auto scalar_members_of(){
    constexpr auto m = ^^T;
    return std::define_static_array(
        std::meta::nonstatic_data_members_of(
            m, std::meta::access_context::current()
        ) | std::ranges::views::filter(cfg::meta::is_scalar_member)
    );
};
template<typename T>
inline constexpr auto struct_like_members_of(){
    constexpr auto m = ^^T;
    return std::define_static_array(
        std::meta::nonstatic_data_members_of(
            m, std::meta::access_context::current()
        ) | std::ranges::views::filter(cfg::meta::is_struct_like_member)
    );
};
template<typename T>
inline constexpr auto array_like_members_of(){
    constexpr auto m = ^^T;
    return std::define_static_array(
        std::meta::nonstatic_data_members_of(
            m, std::meta::access_context::current()
        ) | std::ranges::views::filter(cfg::meta::is_toml_array_member)
    );
};
} //namespace cfg::meta
#endif // CPP26
