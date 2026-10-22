#pragma once 
#include "concept_helpers.hpp"
#include "logger.hpp"
#include "primitive_types.hpp"
#include <format>
#include <string>
#include <type_traits>
#include <vector>
#include <array>
namespace cfg::traits{
template<typename T>
inline constexpr bool is_array_type = false;

template<typename T, typename Alloc>
inline constexpr bool is_array_type<std::vector<T,Alloc>> = true;

template<typename T, std::size_t size>
inline constexpr bool is_array_type<std::array<T,size>> = true;

template<typename T>
inline constexpr bool is_bounded_array_type = false;

template<typename T, std::size_t size>
inline constexpr bool is_bounded_array_type<std::array<T,size>> = true;

}//NOTE: namespace cfg::traits

namespace cfg::concepts{

template<typename T>
concept is_string_type = same_as_nocvref<T, std::string>;

template<typename T>
concept is_scalar_integral =
same_as_any_nocvref<T,
    u8, u16, u32, u64,      // maps to toml integer (int64_t)
    i8, i16, i32, i64      // maps to toml integer (int64_t)
>;
template<typename T>
concept is_enum = std::is_enum_v<T>;

template<typename T>
concept is_scoped_enum = std::is_scoped_enum_v<T>;

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

}// NOTE: namespace cfg::concepts 
namespace toml{
enum struct Kind{
    Unsupported=0,
    Int,
    Float,
    Bool,
    String,
    Enum,
    Array,
    Table
};
template<typename T>
constexpr auto kind_of() -> toml::Kind{
    using U = nocvref<T>;
    if constexpr (std::same_as<U,bool>){
        return Kind::Bool;
    } else if constexpr(cfg::concepts::is_scalar_integral<U>){
        return Kind::Int;
    } else if constexpr(cfg::concepts::is_scalar_floating_point<U>){
        return Kind::Float;
    } else if constexpr(std::same_as<U, std::string>){
        return Kind::String;
    } else if constexpr(std::is_enum_v<U>){ //TODO: implement enums
        static_assert(false, "enums not supported yet");
        return Kind::Enum;
    }else if constexpr (cfg::traits::is_array_type<U>){
        return Kind::Array;
    }else if constexpr (std::is_aggregate_v<U> && std::is_class_v<U>){
        return Kind::Table;
    }else {
        return Kind::Unsupported;
    }

}
}// NOTE: namespace toml
namespace cfg::concepts{

template<typename T>
concept is_scalar_type = (
    toml::kind_of<T>() == toml::Kind::Int 
    || toml::kind_of<T>() == toml::Kind::Float
    || toml::kind_of<T>() == toml::Kind::Bool
    || toml::kind_of<T>() == toml::Kind::String
);
template<typename T> 
concept is_array_type = toml::kind_of<T>() == toml::Kind::Array;

template<typename T> 
concept is_struct_like = toml::kind_of<T>() == toml::Kind::Table;


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
constexpr auto serialize_scalar_value(T const& v){
    if constexpr (cfg::concepts::is_scalar_integral<T>){
        if constexpr (same_as_nocvref<u64, T>){
            static constexpr u64 i64_max = std::numeric_limits<i64>::max();
            if (v > i64_max){
                LOG_WARN("Unable to serialize value '{}' : u64 type fields must have values < i64 max. The value has been clamped to i64 max ({}).",i64_max);
                return std::format("{}",i64_max);
            }
        }
        return std::format("{}",v);
    } else if constexpr( cfg::concepts::is_scalar_boolean<T>){
        return std::format("{}", v ? "true" : "false");
    } else if constexpr (cfg::concepts::is_scalar_char<T>){
        return std::format("{}",v);
    }else if constexpr (cfg::concepts::is_scalar_floating_point<T>){
        auto s = std::format("{}",v);
        if (s.contains("e") || s.contains("nan") || s.contains("inf")){
            return s;
        }else if (!s.contains(".")){
            return s+".0";
        }
        return s;
    }else if constexpr(cfg::concepts::is_string_type<T>){
        return std::format("\"{}\"",v);
    }else{
        static_assert(false, "unable to format scalar value");
    }
}


}//NOTE: namespace cfg
