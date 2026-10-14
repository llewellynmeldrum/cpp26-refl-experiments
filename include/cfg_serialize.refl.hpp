#pragma once 
#include <string>
#include <vector>
#include "file_io.hpp"

namespace cfg{
// Serializes a given struct into TOML, using my own style.
// See docs/toml_style.md for more information
template<typename T>
auto serialize_toml(T const& root) -> std::vector<std::byte>{
    auto res = std::string{};
    return io::to_byte_vector(res);
}
