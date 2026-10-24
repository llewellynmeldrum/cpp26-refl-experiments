#pragma once 

#include "common.hpp"
#include "cfg_metadata.hpp"
#include <optional>
#include <string_view>
namespace cfg{
template<typename T>
    LSP_IGNORE(requires is_root_config<T> && is_valid_root_config<T>)
struct Storage{
    using config_type = T;
    T live{}; 
    std::string_view filename;
    std::optional<T> on_disk_cache {std::nullopt}; // the last read version on disk
};
} // namespace cfg 
