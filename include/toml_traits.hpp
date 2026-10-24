#pragma once 
#include "common.hpp"
#if CPP26
#include "cfg_traits.hpp"
#include "meta_helpers.hpp"
#include <meta>

namespace cfg::meta{
//TODO: These are in the wrong header. or this header is poorly named.
//cant decide, fix later

consteval inline auto is_string_type(std::meta::info m) -> bool{
	return satisfies_concept(^^cfg::concepts::is_string_type, m);
}
consteval inline auto is_scalar_floating_point(std::meta::info m) -> bool{
	return satisfies_concept(^^cfg::concepts::is_scalar_floating_point, m);
}


consteval inline auto is_scalar_type(std::meta::info m) -> bool{
	return satisfies_concept(^^cfg::concepts::is_scalar_type, m);
}

consteval inline auto is_array_type(std::meta::info m) -> bool{
	return satisfies_concept(^^cfg::concepts::is_array_type, m);
}

// Synonymous with a 'table type'
consteval inline auto is_struct_like(std::meta::info m) -> bool{
	return satisfies_concept(^^cfg::concepts::is_struct_like, m);
}
consteval inline auto nocvref_alias(std::meta::info m) -> std::meta::info{
	return std::meta::dealias(std::meta::remove_cvref(m));
}

}// NOTE: namespace cfg::meta

// --- formerly cfg_traits.refl.hpp (needs is_scalar_type above) ---
namespace cfg::meta{
template<typename T>
consteval bool struct_of_scalars(){
    template for (constexpr auto m : nonstatic_members_array<T>()){
        constexpr auto t = std::meta::dealias(std::meta::remove_cvref(std::meta::type_of(m)));
        if constexpr (cfg::meta::is_scalar_type(t) == false){
            return false;
        }
    }
    return true;
}
}

namespace cfg::concepts{
    template<typename T> 
    concept struct_of_scalars = cfg::meta::struct_of_scalars<T>();
}
#endif // CPP26
