#pragma once 

#include <SDL3/SDL.h>

#include "imgui_wrapper.hpp"
#include "cfg_helpers.hpp"
#include "metrics.hpp"
#include "cppslop.hpp"
#include "cfg_gui.hpp"
#include "utils.hpp"

FORWARD_DECLARE_STRUCT( Engine );

struct TrackerStage{
    zstring_view name{"n/a"};
    zstring_view hint{"n/a"};
    glm::vec4 gr_fill= util::_01rgb(0,0,0);
    glm::vec4 gr_line= util::_01rgb(0,0,0);
    MTR::DurationTracker tracker{};
};
inline TrackerStage trk_frametime{
    .name = "Frametime",
    .gr_fill = util::_01rgb(196,196,196),
    .gr_line = util::_01rgb(255,255,255),
};
inline TrackerStage trk_input{
    .name = "Input",
    .gr_fill = util::_01rgb(255,0,0),
    .gr_line = util::_01rgb(128,0,0),
};

inline TrackerStage trk_draw{
    .name = "Draw",
    .gr_fill = util::_01rgb(255,255,0),
    .gr_line = util::_01rgb(128,128,0),
};
inline TrackerStage trk_gui{
    .name = "Gui",
    .gr_fill = util::_01rgb(0,255,0),
    .gr_line = util::_01rgb(0,128,0),
};
inline TrackerStage trk_present{
    .name = "Present",
    .gr_fill = util::_01rgb(0,0,255),
    .gr_line = util::_01rgb(0,0,128),
};
struct Gui{
    Gui(Engine const& engine, SDL_Renderer* renderer, SDL_Window* window, f32 main_scale)
        : m_eng(engine)
        , m_renderer(renderer)
        , m_window(window)
        , m_main_scale(main_scale)
    { 
        init(); 
    }
    ~Gui(){
        shutdown();
    }
    Engine const& m_eng; 
    SDL_Renderer* m_renderer;
    SDL_Window* m_window;
    f32 m_main_scale;

    auto init() -> void;
    auto shutdown() ->void;
#if CPP26
    template<typename T>
    auto draw_config_window(cfg::Storage<T> & storage) -> void{
        auto action_is = cfg::edit_config_window(storage);
        if (action_is.reset_to_default){
            storage.live = {};
        }else if (action_is.save_to_disk){
            cfg::save_to_disk(storage);
        }else if (action_is.load_from_disk){
            cfg::load_from_disk(storage);
        }
        if (action_is.edited){
            LOG_DBG("config edited"sv);
        }
    }
#endif // CPP26

    auto draw() -> void;
    u64 m_frame_count{};



    auto framebuffer_scale() -> glm::vec2;
    auto framebuffer_scale_x() -> f32;
    auto framebuffer_scale_y() -> f32;


    auto begin_frame() -> void;
    auto end_frame() -> void;
};


