#pragma once 

#include <optional>
#include <string_view>
namespace cfg{
template<typename T>
struct Storage{
    // TODO: This type should be special somehow, it should be impossible to
    // copy or move. The type itself doesnt hold a unique object (like a file),
    // but it holds the source of truth, which should be singular for consistency

    T live{}; 
    std::string_view filename;
    std::optional<T> on_disk_cache {std::nullopt}; // the last read version on disk
};
} // namespace cfg 
