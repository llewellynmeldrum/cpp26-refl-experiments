#pragma once 
#include <optional>
namespace cfg{

template<typename T>
struct Range{ T min, max; };

struct Note{ char const* txt; };

template<typename T>
struct FieldMetadata{
    using value_type = T;
    std::optional<cfg::Range<T>> m_range{std::nullopt};
    const char* m_key_identifier     {nullptr};
    const char* m_note               {nullptr};
    constexpr auto key() const -> const char* {
        return m_key_identifier;
    }
    constexpr auto range() const -> cfg::Range<T> {
        ASSERT(m_range.has_value());
        return *m_range;
    }
    constexpr auto is_inside_range(T const& val) const -> bool {
        ASSERT(m_range.has_value());
        return (range().min <= val && val <= range().max);
    }
};
} // NOTE: namespace cfg
