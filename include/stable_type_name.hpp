#pragma once 

#include "common.hpp"
#if CPP26
#include <meta>
#endif

#include "primitive_types.hpp"
#include "concept_helpers.hpp"
#include "cppslop.hpp"

template<typename T>
inline consteval auto stable_type_name(){
    #define DECL_STABLE_NAME(U, NAME) if (same_as_nocvref<T,U>) {return NAME ##sv;}
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
#if CPP26
    return display_string_of(remove_cv(^^T));
#endif
}

// NOTE: since std::meta::identifier_of is not stable, we use these
#if CPP26
inline consteval auto stable_type_name_meta(std::meta::info m){
    using namespace std::meta;
    #define DECL_STABLE_NAME(T, NAME) if (remove_cv(m) == dealias(^^T)) {return NAME ##sv;}
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
    return display_string_of(remove_cv(m));
}
#endif 
