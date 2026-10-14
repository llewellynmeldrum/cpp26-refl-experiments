#pragma once 
#include "config_meta.hpp"
#include "cppslop.hpp"
#include "cfg_registration.hpp"
// in config.hpp
struct Config{
    DESC("Vertical field of view.")           RANGE(5.0f, 120.0f)   f32 fov{10.0f};
    DESC("Camera movement speed multiplier.") RANGE(0.1f, 10.0f)    f32 move_speed{0.0f};
};

template<>
struct cfg::Registration<Config>{
    using ConfigType = Config;
    static constexpr auto config_filename = "config.toml"sv; 
    static constexpr auto config_name = "Config"sv; 
};
