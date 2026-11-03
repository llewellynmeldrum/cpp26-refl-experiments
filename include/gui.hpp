#pragma once 

#include <SDL3/SDL.h>

#include "imgui_wrapper.hpp"
#include "cfg_helpers.hpp"
#include "metrics.hpp"
#include "cppslop.hpp"
#include "cfg_gui.hpp"

FORWARD_DECLARE_STRUCT( Engine );

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



    auto framebuffer_scale() -> glm::vec2;
    auto framebuffer_scale_x() -> f32;
    auto framebuffer_scale_y() -> f32;


    auto begin_frame() -> void;
    auto end_frame() -> void;
};


