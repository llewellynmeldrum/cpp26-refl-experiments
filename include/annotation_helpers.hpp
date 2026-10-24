#pragma once 
#include "common.hpp"
#include <meta>
#if CPP26

template<typename AnnoType>
consteval auto try_get_annotation_of(std::meta::info m) -> std::optional<AnnoType>{
    auto v = annotations_of_with_type(m, ^^AnnoType);
    if (v.empty()){
        return std::nullopt;
    }else{
        return std::meta::extract<AnnoType>(v[0]);
    }
}
template<typename AnnoType>
consteval auto has_annotation(std::meta::info m) -> bool{
    auto v = annotations_of_with_type(m, ^^AnnoType);
    return !v.empty();
}
#endif // CPP26
