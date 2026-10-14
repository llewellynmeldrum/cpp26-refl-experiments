#pragma once 
#include "concept_helpers.hpp"
#include "primitive_types.hpp"
#include <format>
#include <string>
#include <vector>
namespace cfg::traits{
template<typename T>
inline constexpr bool is_array_type = false;

template<typename T, typename Alloc>
inline constexpr bool is_array_type<std::vector<T,Alloc>> = true;

template<typename T, std::size_t size>
inline constexpr bool is_array_type<std::array<T,size>> = true;

}//NOTE: namespace cfg::traits

namespace cfg::concepts{

template<typename T>
concept is_string_type = std::same_as<T,char const*> ||
same_as_any_nocvref<T,
    std::string, 
    std::string_view 
>;

template<typename T>
concept is_scalar_integral =
same_as_any_nocvref<T,
    u8, u16, u32, u64,      // maps to toml integer (int64_t)
    i8, i16, i32, i64      // maps to toml integer (int64_t)
>;
template<typename T>
concept is_scalar_floating_point = 
same_as_any_nocvref<T,
    f16, f32, f64           // maps to toml float   (float64_t)
>;

template<typename T>
concept is_scalar_boolean =
same_as_any_nocvref<T,
    bool
>;
template<typename T>
concept is_scalar_char=
same_as_any_nocvref<T,
    char 
>;

template<typename T>
concept is_scalar_type = 
        is_string_type<T> 
    || is_scalar_boolean<T> 
    || is_scalar_char<T> 
    || is_scalar_integral<T>
    || is_scalar_floating_point<T>;

template<typename T>
concept is_array_type = cfg::traits::is_array_type<nocvref<T>>;

// Synonymous with a 'table type'
template<typename T>
concept is_struct_like = !is_array_type<T> && !is_scalar_type<T> && !is_string_type<T>;



//NOTE: cfg::concepts static tests
//------------------------------------
struct Cfg{
    int foo;
    float bar;
    std::string faahhh;
    struct CfgInner{
        const char* txt;
        bool flag;
    };
};


#define T Cfg
static_assert(!      is_string_type<T>);
static_assert(!      is_scalar_type<T>);
static_assert(!       is_array_type<T>);
static_assert(        is_struct_like<T>);
#undef T
#define T std::array<Cfg,10>
static_assert(!      is_string_type<T>);
static_assert(!      is_scalar_type<T>);
static_assert(        is_array_type<T>);
static_assert(!       is_struct_like<T>);

#undef T
#define T std::array<int,10>
static_assert(!      is_string_type<T>);
static_assert(!      is_scalar_type<T>);
static_assert(        is_array_type<T>);
static_assert(!       is_struct_like<T>);

#undef T
#define T std::vector<std::string>
static_assert(!      is_string_type<T>);
static_assert(!      is_scalar_type<T>);
static_assert(        is_array_type<T>);
static_assert(!       is_struct_like<T>);

#undef T
#define T std::vector<std::vector<Cfg>>
static_assert(!      is_string_type<T>);
static_assert(!      is_scalar_type<T>);
static_assert(        is_array_type<T>);
static_assert(!       is_struct_like<T>);
#undef T


}// NOTE: namespace cfg::concepts

    
//
namespace cfg{

template<typename T>
constexpr auto serialize_value(T const& v){
    if constexpr (cfg::concepts::is_scalar_integral<T>){
        return std::format("{}",v);
    } else if constexpr( cfg::concepts::is_scalar_boolean<T>){
        return std::format("{}", v ? "true" : "false");
    } else if constexpr (cfg::concepts::is_scalar_char<T>){
        return std::format("{}",v);
    }else if constexpr (cfg::concepts::is_scalar_floating_point<T>){
        auto s = std::format("{}",v);
        if (!s.contains(".")){
            return s+".0";
        }
    }else{
        static_assert(false, "unable to format value");
    }
}


}//NOTE: namespace cfg
