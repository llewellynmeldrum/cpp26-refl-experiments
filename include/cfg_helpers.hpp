#pragma once 
#include <concepts>
#include <string_view>

#include "cfg_registration.hpp"
#include "concept_helpers.hpp"

namespace cfg{

template<typename T>
concept valid_registration = requires(T v){
    sizeof(Registration<T>); // check if the specialization exists at all

    {Registration<T>::config_filename} -> same_as_nocvref<std::string_view>;
    {Registration<T>::config_name} -> same_as_nocvref<std::string_view>;

    requires Registration<T>::config_filename.ends_with(".toml");
};

template<typename T> requires cfg::valid_registration<T>
auto get_default() -> T { return T{}; }
} // namespace cfg
