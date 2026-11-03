#pragma once 

#include "primitive_types.hpp"
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>

namespace util{
inline constexpr auto _01rgb(u8 r, u8 g, u8 b, u8 a=255) -> glm::vec4{
    return {r/255.0, g/255.0, b/255.0, a/255.0};
}
};

