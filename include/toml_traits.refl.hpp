#pragma once 
#include "toml_traits.hpp"
#include "meta_helpers.refl.hpp"
#include <meta>

namespace cfg::meta{

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

}// NOTE: namespace cfg::meta
