#pragma once 
#include "meta_helpers.refl.hpp"
#include "cfg_traits.hpp"
#include <meta>
namespace cfg::meta{
template<typename T>
consteval bool struct_of_scalars(){
    template for (constexpr auto m : nonstatic_members_array<T>()){
        constexpr auto t = std::meta::dealias(std::meta::remove_cvref(std::meta::type_of(m)));
        if constexpr (cfg::meta::is_scalar_type(t) == false){
            return false;
        }
    }
    return true;
}
}

namespace cfg::concepts{
    template<typename T> 
    concept struct_of_scalars = cfg::meta::struct_of_scalars<T>();
}
