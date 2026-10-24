#pragma once 

#include <string_view>
#include "meta_shorthand.hpp"
#if CPP26
#include "primitive_types.hpp"
#include "stable_type_name.hpp"
using namespace std::string_view_literals;
#define COMPTIME_STR(s) (std::define_static_string(s))
#define COMPTIME_ARR(a) (std::define_static_array(a))

template<typename T> 
inline consteval auto comptime_arr(T const& v) -> decltype(auto){
    return std::define_static_array(v);
}
template<typename T> 
inline consteval auto comptime_str(T const& v) -> decltype(auto){
    return std::define_static_string(v);
}

template<typename T>
inline consteval auto is_annotation_of(std::meta::info m) -> bool{
    using namespace std::meta;
    auto type = dealias(remove_cvref(type_of(m)));
    return type == ^^T;
}

template<typename T>
inline constexpr auto nonstatic_members_array(){
    return std::define_static_array(
        std::meta::nonstatic_data_members_of(
            ^^T, std::meta::access_context::current()
        )
    );
};
template<std::meta::info m>
inline constexpr auto annotations_array(){
    return std::define_static_array(
        std::meta::annotations_of(
            m
        )
    );
};

template<typename T>
static constexpr auto static_members_array(){
    return std::define_static_array(
        std::meta::static_data_members_of(
            ^^T, std::meta::access_context::current()
        )
    );
};

struct ReflectionComment{ char const* text; };
struct FieldRename{ char const* new_name; };




template<typename T, typename Fn>
[[nodiscard]] consteval auto nonstatic_members_partitioned(Fn&& pred){
    auto out = std::vector<std::meta::info>{};
    template for (constexpr auto m: nonstatic_members_array<T>()){
        if (pred(m)){
            out.push_back(m);
        }
    }
    template for (constexpr auto m: nonstatic_members_array<T>()){
        if (!pred(m)){
            out.push_back(m);
        }
    }
    return std::define_static_array(out);
}

template<typename T, typename Fn>
[[nodiscard]] consteval auto nonstatic_members_if(Fn&& pred){
    auto out = std::vector<std::meta::info>{};
    template for (constexpr auto m: nonstatic_members_array<T>()){
        if (pred(m)){
            out.push_back(m);
        }
    }
    return std::define_static_array(out);
}

template<typename ...Args>
inline consteval auto satisfies_concept(meta::info r_concept, Args ...args){
    return meta::extract<bool>(meta::substitute(r_concept, {args...}));
}
#endif // CPP26
