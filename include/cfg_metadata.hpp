#pragma once 
#include "primitive_types.hpp"
#include <format>
#include <libassert/assert.hpp>
#include <optional>

#if !(defined(__cpp_impl_reflection) && defined(__cpp_lib_reflection))
    #define RANGE(min,max) 
    #define NOTE(txt) 
    #define SKIP()  
#else
    #include <meta>
    #define RANGE(min, max) [[=cfg::Range{min,max}]]
    #define NOTE(txt)       [[=cfg::Note{std::define_static_string(txt)}]]
    #define SKIP()          [[=cfg::Skip{}]]
#endif
namespace cfg {
// NOTE: Maybe Range for string should represent the min and max length of the string
// TODO: as a rule, scalar elements of containers should inherit the range of their container
struct Range{
    f64 min, max; 
    template<typename T>
    static constexpr auto default_value()->Range{
        return {
            .min = static_cast<f64>(std::numeric_limits<T>::lowest() / 2.0),
            .max = static_cast<f64>(std::numeric_limits<T>::max() / 2.0),
        };
    }
};
struct Note{ char const* txt; };
struct Skip{ [[maybe_unused]] char const* reason;};

struct RGBA{};
struct RGB{};
struct XYZ{};
struct XYZW{};

} // NOTE: namespace cfg
namespace cfg{
template<typename T>
struct FieldMetadata{
    using value_type = T;
    T                               m_default_val;
    std::optional<cfg::Range>    m_range              {std::nullopt};
    const char*                     m_key_identifier     {nullptr};
    const char*                     m_note               {nullptr};
    bool                            m_should_skip{};
    constexpr auto key() const -> const char* {
        return m_key_identifier;
    }
    constexpr auto range() const -> cfg::Range {
        ASSERT(m_range.has_value());
        return *m_range;
    }
    constexpr auto is_inside_range(T const& val) const -> bool {
        ASSERT(m_range.has_value());
        return (range().min <= val && val <= range().max);
    }
};
} // NOTE: namespace cfg
