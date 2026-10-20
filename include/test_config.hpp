#pragma once 
//#include "config_meta.hpp"
#include "cppslop.hpp"
#include "cfg_registration.hpp"
#include "cfg_metadata.hpp"
// in config.hpp

struct Vector2{
    float x{};
    float y{};
};
struct DisplayConfig{
    DESC("Vertical field of view.") RANGE(5.0f, 120.0f)   
    f32 fov{10.0f};

    DESC("Default window width.") RANGE(640.0f, 3840.0f)   
    f32 window_width_px{1000.0f};

    DESC("Default window height.") RANGE(480.0f, 2160.0f)   
    f32 window_height_px{1000.0f};

    DESC("Default window position (vector).")              
    Vector2 window_pos_px{0.0f,0.0f};
};
struct CameraConfig{
    DESC("Vertical field of view.")           RANGE(5.0f, 120.0f)   f32 fov{10.0f};
};
struct MovementConfig{
    DESC("Camera movement speed multiplier.") RANGE(0.1f, 10.0f)  f32 move_speed{1.0f};
    DESC("Camera pan movement speed multiplier.") RANGE(0.1f, 10.0f)  f32 pan_speed{1.0f};
};
struct PlayerConfig{
    MovementConfig movement;
    CameraConfig camera;
    DisplayConfig display;
};
struct Config {
    int foo = 5;
    PlayerConfig player;
    DESC("These are some floats") std::array<int, 5> integers = {1,2,3,4,5};
    std::array<float, 5> floats = {1,2,3,4,5};
};

template<>
struct cfg::Registration<Config>{
    using ConfigType = Config;
    static constexpr auto config_filename = "config.toml"sv; 
    static constexpr auto config_name = "Config"sv; 
    static constexpr auto description = "Main config struct."sv; 
};
