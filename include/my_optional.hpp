#pragma once 

#include <libassert/assert.hpp>

struct nullopt_t{};
inline constexpr nullopt_t nullopt;
template<typename T>
struct optional{
    constexpr optional(nullopt_t const&) 
        : engaged(false)
    {}
    constexpr optional() 
        : engaged(false)
    {}
    constexpr optional(T const& mv) 
        : v(mv)
    {}
    T v;
    bool engaged;
    bool has_value(){ return engaged;}
    auto& value(){ASSERT(engaged); return v;}
    auto& operator*(){return value();}
};
