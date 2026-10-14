#pragma once 
#include <cstdio>
#include <stdexcept>
#include <string>

template<std::size_t N=256>
struct fixed_string{
    char m_storage[N] = {};
    constexpr fixed_string() = default;
    constexpr fixed_string(std::string s) {
        auto i = 0uz; 
        for (const auto ch: s){
            if (i >= N-1){
                throw std::logic_error("fixed string size exceeded");
            }
            m_storage[i] = ch;
            i++;
        }
    }
    constexpr fixed_string(std::string_view sv) {
        auto i = 0uz; for (const auto ch: sv){
            if (i >= N-1){
                throw std::logic_error("fixed string size exceeded");
            }
            m_storage[i] = ch;
            i++;
        }
    }
    constexpr fixed_string(const char* in) {
        auto i = 0uz;
        while(in[i]!= '\0'){//NOLINT
            if (i >= N-1){
                throw std::logic_error("fixed string size exceeded");
            }
            m_storage[i] = in[i];
            i++;
        }
    }
    constexpr operator std::string() const{
        return std::string(m_storage);
    }
};
static_assert(requires{std::string(fixed_string{});});
