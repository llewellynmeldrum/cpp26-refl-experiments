#pragma once 

#include <optional>
namespace cfg{
template<typename T>
struct Storage{
    // TODO: This type should be special somehow, it should be impossible to
    // copy or move. The type itself doesnt hold a unique object (like a file),
    // but it holds the source of truth, which should be singular for consistency
    Storage(Storage const&) = delete;
    Storage(Storage &&) = delete;
    Storage& operator=(Storage const& ) = delete;
    Storage &operator=(Storage &&) = delete;

    Storage() =default;
    T live{}; 
    std::optional<T> on_disk_cache {std::nullopt}; // the last read version on disk
};
} // namespace cfg 
