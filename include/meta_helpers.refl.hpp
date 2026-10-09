#pragma once 

#include <meta>
namespace meta = std::meta;
#define COMPTIME_STR(s) (std::define_static_string(s))
#define COMPTIME_ARR(a) (std::define_static_array(a))

// NOTE: since identifier_of is not stable, we use thesec
inline consteval auto stable_type_name(meta::info m){
    #define DECL_STABLE_NAME(T, NAME) if (meta::remove_cv(m) == meta::dealias(^^T)) {return NAME ##sv;}
    DECL_STABLE_NAME( i8,  "int8_t")
    DECL_STABLE_NAME(i16, "int16_t")
    DECL_STABLE_NAME(i32, "int32_t")
    DECL_STABLE_NAME(i64, "int64_t")

    DECL_STABLE_NAME( u8, "uint8_t")
    DECL_STABLE_NAME(u16, "uint16_t")
    DECL_STABLE_NAME(u32, "uint32_t")
    DECL_STABLE_NAME(u64, "uint64_t")

    DECL_STABLE_NAME(bool, "bool")
    DECL_STABLE_NAME(f16, "float16")
    DECL_STABLE_NAME(f32, "float32")
    DECL_STABLE_NAME(f64, "float64")

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
static constexpr auto nonstatic_members_array(){
    return std::define_static_array(
        std::meta::nonstatic_data_members_of(
            ^^T, std::meta::access_context::current()
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

#define SRL_DESC(s) [[=ReflectionComment{COMPTIME_STR(s)}]]
#define SRL_RENAME(s) [[=FieldRename{COMPTIME_STR(s)}]]



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
inline consteval auto satisifes_concept(meta::info r_concept, Args ...args){
    return meta::extract<bool>(meta::substitute(r_concept, {args...}));
}
