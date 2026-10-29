#pragma once
#include <utility>
#include "logger.hpp"
#ifndef __cplusplus
	#error "This header is intended for c++ only."
#endif // __cplusplus
#include "concept_helpers.hpp"
#include "bitfield_helpers.hpp"
#include "traits.hpp"

constexpr bool has(u32 bits, auto e) { return (bits & std::to_underlying(e)) == std::to_underlying(e); };

template<typename E> requires is_enum<E>
struct Flags{
    using underlying_t = std::underlying_type_t<E>;
    constexpr Flags() = default;
    constexpr Flags(E e) : m_bits(std::to_underlying(e)){}
    constexpr explicit Flags(underlying_t raw) : m_bits(raw){}

    constexpr underlying_t raw() const noexcept{ return m_bits;}
//    constexpr underlying_t to_bits_enum() const noexcept{ return static_cast<E>(m_bits);}

    constexpr auto has(E e) const -> bool { return bitfield_has_any(m_bits, e);}
    constexpr auto has_only(E e) const -> bool { return bitfield_has_only(m_bits, e);}
    constexpr auto operator|=(this auto&& self, Flags rhs)-> auto& { return self = (self | rhs); }
    constexpr auto operator&=(this auto&& self, Flags rhs)-> auto& { return self = (self & rhs); }
    constexpr auto operator^=(this auto&& self, Flags rhs)-> auto& { return self = (self ^ rhs); }
    constexpr auto operator==(this auto&& self, Flags rhs)-> bool { return self.m_bits == rhs.m_bits;}
    constexpr auto operator!=(this auto&& self, Flags rhs)-> bool { return self.m_bits != rhs.m_bits;}

    constexpr inline Flags operator~() const{ return Flags(~m_bits); }

    underlying_t m_bits{0};
};

template<typename E>
    requires is_enum<E>
constexpr inline auto operator|(Flags<E> lhs, Flags<E> rhs){
    return Flags<E>(lhs.raw() | rhs.raw());
};
template<typename E>
    requires is_enum<E>
constexpr inline auto operator&(Flags<E> lhs, Flags<E> rhs){
    return Flags<E>(lhs.raw() & rhs.raw());
};
template<typename E>
    requires is_enum<E>
constexpr inline auto operator^(Flags<E> lhs, Flags<E> rhs){
    return Flags<E>(lhs.raw() ^ rhs.raw());
};

#if CPP26
namespace traits{
    struct enable_enum_bitwise_t{};
};
#if CLANGD_PARSE
#define TRAITS_ENABLE_ENUM_BITWISE 
#define ENUM_WITH_BITWISE enum struct 
#else
#define TRAITS_ENABLE_ENUM_BITWISE [[=traits::enable_enum_bitwise_t{}]]
#define ENUM_WITH_BITWISE enum struct TRAITS_ENABLE_ENUM_BITWISE
#endif

template<typename T> requires is_enum<T>
constexpr inline bool enable_enum_bitwise = traits::concepts::type_has_trait<T, traits::enable_enum_bitwise_t>;

// Enable bitwise OR between members of the same enum which are signed up for enum_bitwise_or_opt_in
template<typename E> 
    requires is_enum<E> && enable_enum_bitwise<E>
constexpr inline auto operator|(E lhs, E rhs){
    return Flags<E>(std::to_underlying(lhs) | std::to_underlying(rhs));
};
template<typename E> 
    requires is_enum<E> && enable_enum_bitwise<E>
constexpr inline auto operator&(E lhs, E rhs){
    return Flags<E>(std::to_underlying(lhs) & std::to_underlying(rhs));
};
template<typename E> 
    requires is_enum<E> && enable_enum_bitwise<E>
constexpr inline auto operator^(E lhs, E rhs){
    return Flags<E>(std::to_underlying(lhs) ^ std::to_underlying(rhs));
};

#endif

#define MANUAL_ENABLE_ENUM_BITWISE(ENUM) template<> inline constexpr bool enable_enum_bitwise_or<ENUM> = true;


//template<typename T>
//concept is_bitmask_flag = requires(T t){
//    typename T::underlying_t;
//    t.m_bits;
//    requires std::same_as<decltype(t.m_bits),typename T::underlying_t>;
//};


