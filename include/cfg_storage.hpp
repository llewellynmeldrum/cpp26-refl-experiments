#pragma once 

#include "common.hpp"
#if CPP26
#include "cfg_metadata.hpp"
#endif 
#include <optional>
#include <string_view>
namespace cfg{
template<typename T>
    LSP_IGNORE(requires valid_config_struct<T>)
struct Storage{
    using config_type = T;
    T live{}; 
    std::string_view filename;
    std::optional<T> on_disk_cache {std::nullopt}; // the last read version on disk
};
} // namespace cfg 
