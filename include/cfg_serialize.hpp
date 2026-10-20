#pragma once 
#include <string>
#include <vector>
#include "cfg_metadata.hpp"
#include "cfg_registration.hpp"
#include "file_io.hpp"
#include "cfg_serialize.refl.hpp"

namespace cfg{
// THINKING: 
// i want to have some sort of IR. 
// I thought about a sort of tree of FieldContext's, but its templated. 
// Could i remove the template somehow, but retain:
// - the default value
// What if instead, we walk through the 

// BRIEF: Serializes a given struct into TOML, using my own pretty opinionated style.
// See docs/toml_style.md for more information
// include/test_config.hpp
template<typename T>
auto serialize_toml(T const& root) -> std::vector<std::byte>{
    using Reg = cfg::Registration<T>;
    auto res = serialize_root_table(root);
    return io::to_byte_vector(res);
}
}// NOTE: namespace cfg
