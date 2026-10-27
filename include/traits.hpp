#pragma once 
#if CPP26
#include <meta>
#include "annotation_helpers.hpp"
#include "common.hpp"
namespace traits{
    template<typename TraitType>
    consteval auto type_has_trait(std::meta::info type) -> bool{
        return has_annotation<TraitType>(type);
    }
namespace concepts{
template<typename T, typename TraitType>
concept type_has_trait = type_has_trait<TraitType>(^^T);

} // namespace concepts
} // namespace traits
#endif
