#pragma once 
#include "cfg_traits.hpp"
#include "meta_helpers.refl.hpp"
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
