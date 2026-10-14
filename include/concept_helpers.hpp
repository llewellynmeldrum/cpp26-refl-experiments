#pragma once
#include <concepts>
#include <type_traits>

template<typename T>
using nocvref = std::remove_cvref_t<T>;

template<typename T>
using noref = std::remove_reference_t<T>;


template<typename T, typename U>
concept same_as_nocvref = std::same_as<nocvref<T>,nocvref<U>>;

template<typename T, typename U>
concept same_as_noref = std::same_as<noref<T>,noref<U>>;


// NOTE: Variadic concepts

template<typename T, typename ...Ts>
concept same_as_any = (std::same_as<T, Ts> || ...);

template<typename T, typename ...Ts>
concept same_as_all = (std::same_as<T, Ts> && ...);

template<typename T, typename ...Ts>
concept same_as_none = ((!std::same_as<T, Ts>) && ...);



template<typename T, typename ...Ts>
concept same_as_any_nocvref = (same_as_nocvref<T, Ts> || ...);

template<typename T, typename ...Ts>
concept same_as_all_nocvref = (same_as_nocvref<T, Ts> && ...);

template<typename T, typename ...Ts>
concept same_as_none_nocvref = ((!same_as_nocvref<T, Ts>) && ...);



// NOTE: Numeric concepts
template<typename T>
concept is_numeric = std::floating_point<T> || std::integral<T>;
