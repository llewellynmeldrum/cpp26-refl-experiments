#pragma once 
#include "span_wrapper.hpp"
#include <string>

struct zstring_view{
    // NOTE: No std::string_view ctor on purpose.
    using char_t = char;
    const_dyn_span<char_t> m_span;

    constexpr zstring_view(const char* cstr): m_span(cstr,std::char_traits<char_t>::length(cstr)) {}
    constexpr zstring_view(const char* cstr, size_t len): m_span(cstr,len) {}
    constexpr zstring_view(std::string const& str ): m_span(str.c_str(),str.length()) {}
    constexpr explicit operator std::string_view() const {return std::string_view{m_span};}

    constexpr auto cstr() const -> const char*{return m_span.data();}
    constexpr auto empty() const -> bool {return m_span.empty();}
    constexpr auto size() const -> bool {return m_span.size();}
    constexpr auto length() const -> bool {return m_span.size();}
};


namespace zstring_view_literals {
#pragma GCC diagnostic push
#if CLANGD_PARSE
    #pragma GCC diagnostic ignored "-Wreserved-user-defined-literal"
    #pragma GCC diagnostic ignored "-Wuser-defined-literals"
#else
    #pragma GCC diagnostic ignored "-Wliteral-suffix"
#endif
    inline constexpr zstring_view
    operator""_zv(const char* cstr, size_t len) noexcept {
        return zstring_view{cstr, len}; 
    }
#pragma GCC diagnostic pop
static constexpr auto zstr = "hi"_zv;
} // namespace zstring_literals

