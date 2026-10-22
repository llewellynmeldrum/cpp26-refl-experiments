#pragma once 
//#include "config_meta.hpp"
#include "cppslop.hpp"
#include "cfg_registration.hpp"
#include "cfg_metadata.hpp"
#include "cfg_traits.hpp"
// in config.hpp

struct Vector2{
    float x{};
    float y{};
};
struct DisplayConfig{
    NOTE("Vertical field of view.") RANGE(5.0f, 120.0f)   
    f32 fov{10.0f};

    NOTE("Default window width.") RANGE(640.0f, 3840.0f)   
    f32 window_width_px{1000.0f};

    NOTE("Default window height.") RANGE(480.0f, 2160.0f)   
    f32 window_height_px{1000.0f};

    NOTE("Default window position (vector).")              
    Vector2 window_pos_px{0.0f,0.0f};
};
struct CameraConfig{
    NOTE("Vertical field of view.")           RANGE(5.0f, 120.0f)   f32 fov{10.0f};
};
struct MovementConfig{
    NOTE("Camera movement speed multiplier.") RANGE(0.1f, 10.0f)  f32 move_speed{1.0f};
    NOTE("Camera pan movement speed multiplier.") RANGE(0.1f, 10.0f)  f32 pan_speed{1.0f};
};
struct PlayerConfig{
    MovementConfig movement;
    CameraConfig camera;
    DisplayConfig display;
};
struct Config {
    std::string str{"hi"};
    RANGE(0, 100) i32 foo = 5;
    PlayerConfig player;
    NOTE("These are some floats") std::array<int, 5> integers = {1,2,3,4,5};
    std::array<float, 5> floats = {1,2,3,4,5};
    std::array<MovementConfig,5 > move_cfgs= {};
};

REGISTER_CONFIG(
    Config,
    "Config", 
    "Main config struct."
);

