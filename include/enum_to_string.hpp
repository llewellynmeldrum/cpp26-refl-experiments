#pragma once 


#include "common.hpp"
#if CPP26
#include <meta>
#include <vector>
#include <flat_map>
#include <ranges>
#include <utility>
#include <optional>
#include "annotation_helpers.hpp"
#include "zstring_view.hpp"
#include "cfg_annotations.hpp"

// TODO: big problem. the flat map is not a structural type.
// I think we will have to instead do index shenaigans. Maybe we only allow enums which are not set to a custom value?
// This would disallow flag type enums which is a bit sad, but we could always 
// just have a separate specialization for those, that can be marked as such with an annotation.

namespace impl{
template<std::meta::info m>
inline consteval auto enum_name(){
    constexpr auto has_rename = try_get_annotation_of<cfg::Name>(m);
    if constexpr (has_rename){
        return (std::define_static_string(std::string_view((*has_rename).txt)));
    }else{
        return (define_static_string(identifier_of(m)));
    }
}
template<typename T> requires std::is_enum_v<T>
consteval auto get_enum_members_list(){
    auto res = std::vector<T>{};
    template for (constexpr auto m: define_static_array(enumerators_of(^^T))){
        res.emplace_back([:m:]);
    }
    return std::define_static_array(res);
}
} // namespace impl 



namespace impl {
template<typename T> requires std::is_enum_v<T>
inline consteval auto get_enum_name_list(){
    using namespace std::meta;
    auto res = std::vector<const char*>{};
    template for (constexpr auto m: define_static_array(enumerators_of(^^T))){
        res.push_back(enum_name<m>());
    }
    return std::define_static_array(res);
}
} // namespace impl
template<typename T> requires std::is_enum_v<T>
constexpr inline auto enum_value_list = impl::get_enum_members_list<T>();

template<typename T> requires std::is_enum_v<T>
inline constexpr auto enum_name_list = impl::get_enum_name_list<T>();

template<typename T>
inline constexpr auto index_of_enum_member(const T& v){
    return std::to_underlying(v);
}

template<typename T>
inline constexpr auto enum_size(){
    return enumerators_of(^^T).size();
}
template<typename T>
inline constexpr auto name_of_enum_member(const T& v){
    return enum_name_list<T>.at(index_of_enum_member(v));
}
template<typename T>
inline constexpr auto try_lookup_enum_name(std::string_view sv)-> std::optional<T>{
    for (auto i = 0uz; i<enum_size<T>(); i++){
        auto const& name = enum_name_list<T>.at(i);
        if (name == sv){
            return enum_value_list<T>.at(i);
        }
    }
    return std::optional<T>{};
}
#endif //CPP26
