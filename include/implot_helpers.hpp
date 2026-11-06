#pragma once 
#include <concepts>
#include "glm/vec2.hpp"
#include "glm/vec4.hpp"


template<typename T>
inline auto ImVec(T const& v){
    if constexpr(std::same_as<T,glm::vec2>){
        return ImVec2(v.x,v.y);
    }else if constexpr(std::same_as<T,glm::vec4>){
        return ImVec4(v.x,v.y,v.z,v.w);
    }else{
        static_assert(false,"Vector has no ImVec equivalent.");
    }
}
