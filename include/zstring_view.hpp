#pragma once 
#include "span_wrapper.hpp"


struct cstring_view{
    using char_t = char;
    const_dyn_span<char_t> m_span;
    cstring_view(const char* cstr, size_t len):
        m_span(cstr,len)
    {}
};

namespace cstring_view_literals {
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wliteral-suffix"
#pragma GCC diagnostic ignored "-Wreserved-user-defined-literal"
#pragma GCC diagnostic ignored "-Wuser-defined-literals"
    inline constexpr cstring_view
    operator""cv(const char* cstr, size_t len) noexcept {
        return cstring_view{cstr, len}; 
    }
#pragma GCC diagnostic pop
} // namespace cstring_literals
