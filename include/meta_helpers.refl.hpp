#pragma once 

#include "meta_shorthand.refl.hpp"
#include "primitive_types.hpp"
#include "stable_type_name.hpp"
#include <string_view>
#include <meta>
using namespace std::string_view_literals;
#define COMPTIME_STR(s) (std::define_static_string(s))
#define COMPTIME_ARR(a) (std::define_static_array(a))

// NOTE: since std::meta::identifier_of is not stable, we use these
inline consteval auto stable_type_name(meta::info m){
    #define DECL_STABLE_NAME(T, NAME) if (meta::remove_cv(m) == meta::dealias(^^T)) {return NAME ##sv;}
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

    #undef DECL_STABLE_NAME
    return meta::display_string_of(meta::remove_cv(m));
}
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
