
#pragma once 
//#include "config_meta.hpp"
#include "cfg_annotations.hpp"
#include "common.hpp"
#include "cppslop.hpp"
#include "cfg_registration.hpp"
#include "cfg_metadata.hpp"
#include "cfg_traits.hpp"
// in config.hpp

#include "auto_format.hpp"
struct AUTO_FORMAT Vector2{
    float x{};
    float y{};
}; 

struct AUTO_FORMAT CameraSettings{
    CFG_NOTE("Vertical field of view.") CFG_RANGE(5.0f, 120.0f)  
    f32 fov{10.0f};

    CFG_NOTE("Camera pan movement speed multiplier (mouse).") CFG_RANGE(0.1f, 10.0f)  
    f32 mouse_sensitivity_x{1.0f};

    CFG_NOTE("Camera pan movement speed multiplier (mouse).") CFG_RANGE(0.1f, 10.0f)  
    f32 mouse_sensitivity_y{1.0f};

    CFG_NOTE("Display size (px).") 
    Vector2 display_extent_px{1024.0f,768.0f};
}; 

struct AUTO_FORMAT Player{
    CFG_NOTE("Players name.") 
    std::string name{"n/a"};

    CFG_NOTE("Player movement speed multiplier") CFG_RANGE(0.0f,10.0f)
    f32 movespeed{1.0f};

    Vector2 spawn_pos{0.0f,0.0f};
    CameraSettings camera_settings;

// TODO: enable this to test range being inherited by children. i think i only enabled it for arrays
//    CFG_NOTE("Player spawn position") CFG_RANGE(0.0f,10.0f)
};

struct AUTO_FORMAT Enemy{
    CFG_NOTE("Enemy name")
    std::string name{};

    CFG_NOTE("Enemy movement speed multiplier") CFG_RANGE(0.0f,10.0f)
    f32 movespeed{1.0f};
};

enum struct AUTO_FORMAT NoiseType{
     Perlin CFG_NOTE("this is perlin!!!!"),
    Simplex CFG_NOTE("Im just another soul for sale, oh well."),
     Worlei CFG_NOTE("Kid named worlei"),
};
struct AUTO_FORMAT NoiseCfg{
    CFG_NOTE("Noise type, eg perlin, simplex")
    NoiseType noise_type{};

    CFG_NOTE("Noise frequency.") CFG_RANGE(0.0f,2.0f)
    f32 freq{1.0f};

    CFG_NOTE("Number of octaves.") CFG_RANGE(1,10)
    i32 octave_count{1};

    CFG_NOTE("Fractal type, eg ridged, fractional brownian motion (fBm)") 
    std::string fractal_type{"fBm"};

    CFG_NOTE("Fractal lacunarity, i.e the rate of frequency increase of each octave") CFG_RANGE(0.0f,10.0f)
    f32 fractal_lacunarity{2.0f};
};

struct AUTO_FORMAT CFG_REGISTER_ROOT_CONFIG("Example Config", "Contains a few odd fields.")
Config {
    CFG_NOTE("This is a hint")
    std::string str{"random kid:"};
    Player player;
    CFG_NOTE("Noise configuration")
    NoiseCfg noise;

    CFG_NOTE("Name list")
    std::array<std::string, 5> names = {
        "hientists",
        "have",
        "discovered",
        "a",
        "sci",
    };
    CFG_NOTE("Enemy list") 
    std::array<Enemy, 5> enemies = {
        Enemy{.name = "hientists"},
        Enemy{.name = "have"},
        Enemy{.name = "discovered"},
        Enemy{.name = "a"},
        Enemy{.name = "sci"},
    };
};

