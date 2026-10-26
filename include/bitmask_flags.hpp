#pragma once
#ifndef __cplusplus
	#error "This header is intended for c++ only."
#endif // __cplusplus
#include "concept_helpers.hpp"
#include "bitfield_helpers.hpp"
#include <utility>

constexpr bool has(u32 bits, auto e) { return (bits & std::to_underlying(e)) == std::to_underlying(e); };

template<typename E> requires is_enum<E>
struct Flags{
    using underlying_t = std::underlying_type_t<E>;
    constexpr Flags() = default;
    constexpr Flags(E e) : m_bits(std::to_underlying(e)){}
    constexpr explicit Flags(underlying_t raw) : m_bits(raw){}

    constexpr underlying_t raw() const noexcept{ return m_bits;}

    constexpr bool has(E e) const{ return bitfield_has_any(m_bits, e);};
    constexpr Flags& operator|=(Flags rhs){ return *this = (*this | rhs); }
    constexpr Flags& operator&=(Flags rhs){ return *this = (*this & rhs); }
    constexpr Flags& operator^=(Flags rhs){ return *this = (*this ^ rhs); }
    constexpr bool operator==(Flags rhs){return m_bits == rhs.m_bits;}

    constexpr inline Flags operator~() const{ return Flags(~m_bits); }

    underlying_t m_bits{0};
};

template<typename E>
    requires is_enum<E>
constexpr inline auto operator|(Flags<E> lhs, Flags<E> rhs){
    return Flags<E>(std::to_underlying(lhs) | std::to_underlying(rhs));
};
template<typename E>
    requires is_enum<E>
constexpr inline auto operator&(Flags<E> lhs, Flags<E> rhs){
    return Flags<E>(std::to_underlying(lhs) & std::to_underlying(rhs));
};
template<typename E>
    requires is_enum<E>
constexpr inline auto operator^(Flags<E> lhs, Flags<E> rhs){
    return Flags<E>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
};

template<typename T>
struct enum_bitwise_or_opt_in{
    static constexpr bool enabled = false;
};
template<typename T>
inline constexpr bool enable_enum_bitwise_or = false;

#define ENABLE_ENUM_BITWISE(ENUM) template<> inline constexpr bool enable_enum_bitwise_or<ENUM> = true;

// Enable bitwise OR between members of the same enum which are signed up for enum_bitwise_or_opt_in
template<typename E> requires is_enum<E> && enable_enum_bitwise_or<E>
constexpr inline auto operator|(E lhs, E rhs){
    return Flags<E>(std::to_underlying(lhs) | std::to_underlying(rhs));
};


template<typename T>
concept is_bitmask_flag = requires(T t){
    typename T::underlying_t;
    t.m_bits;
    requires std::same_as<decltype(t.m_bits),typename T::underlying_t>;
};


// EXAMPLE:
enum struct _FooBits: u32{
    eFlag_A = 1 << 0,
    eFlag_B = 1 << 1,
    eFlag_AB = (eFlag_A | eFlag_B),
};
ENABLE_ENUM_BITWISE(_FooBits);
using _Foo = Flags<_FooBits>;
static_assert(_Foo{ _FooBits::eFlag_A | _FooBits::eFlag_B } == _Foo{_FooBits::eFlag_AB});
static_assert(_Foo{ 1 | 2 } == _Foo{_FooBits::eFlag_AB});
