#pragma once 
#include <format>
#include <optional>

#ifdef CLANGD_PARSE
    #define RANGE(min,max) 
    #define DESC(txt) 
    #define DONT_SERIALIZE() 
#else
    #include <meta>
    #define RANGE(min, max) [[=cfg::Range{min,max}]]
    #define DESC(txt)       [[=cfg::Note{std::define_static_string(txt)}]]
#endif
namespace cfg{
template<typename T>
struct Range{ T min, max; };
struct Note{ char const* txt; };
struct Skip{ [[maybe_unused]] char const* reason;};

} // NOTE: namespace cfg
template<typename T>
struct std::formatter<cfg::Range<T>>{
    constexpr auto parse(std::format_parse_context& ctx) {return ctx.begin();}
    constexpr auto format(const cfg::Range<T>& obj, std::format_context& ctx) const noexcept{
        return std::format_to(ctx,"[{},{}]",obj.min,obj.max);
    }
};
namespace cfg{
template<typename T>
struct FieldMetadata{
    using value_type = T;
    T                               m_default_val;
    std::optional<cfg::Range<T>>    m_range              {std::nullopt};
    const char*                     m_key_identifier     {nullptr};
    const char*                     m_note               {nullptr};
    bool                            m_should_skip{};
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
