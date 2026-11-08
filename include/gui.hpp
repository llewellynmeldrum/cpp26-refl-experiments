#pragma once 

#include <SDL3/SDL.h>

#include "imgui_wrapper.hpp"
#include "cfg_helpers.hpp"
#include "metrics.hpp"
#include "cppslop.hpp"
#include "cfg_gui.hpp"
#include "utils.hpp"

FORWARD_DECLARE_STRUCT( Engine );

enum struct OverlayMode{
    Hidden CFG_NOTE("All graphs are hidden."),
    Minimal CFG_NOTE("A small sparkline frametime graph is shown."),
    BreakdownOnly CFG_NOTE("A full sized breakdown of subsystem contribution to frametime is shown."),
    Full CFG_NOTE("Everything is shown."),
};
struct GuiDrawContext{
    OverlayMode overlay_mode;
    glm::vec2 window_extent = glm::vec2{ImGui::GetMainViewport()->WorkSize};
    auto uv2logical_extent(glm::vec2 uv) const noexcept -> glm::vec2{
        ASSERT(uv.x >= 0 && uv.y >= 0);
        return uv * window_extent;
    }
};


FORWARD_DECLARE_STRUCT(ImPlotItem);

struct TrackerStage{
    zstring_view name{"n/a"};
    zstring_view hint{"n/a"};
    glm::vec4 gr_fill= util::_01rgb(0,0,0);
    auto gr_line() const noexcept -> glm::vec4{
        return gr_fill * 0.85f;
    }
    MTR::DurationTracker tracker{};
    auto im_id() const noexcept -> std::string;
    auto check_enabled() noexcept -> bool;
    bool was_enabled_this_frame = false;
};
inline TrackerStage trk_frametime{
    .name = "Frametime",
    .gr_fill = util::_01rgb(196,196,196),
};
inline TrackerStage trk_input{
    .name = "Input",
    .gr_fill = util::_01rgb(230, 81, 66),
};

inline TrackerStage trk_draw{
    .name = "Draw",
    .gr_fill = util::_01rgb(245, 239, 12),
};
inline TrackerStage trk_gui{
    .name = "Gui",
    .gr_fill = util::_01rgb(36, 235, 35),
};
inline TrackerStage trk_present{
    .name = "Present",
    .gr_fill = util::_01rgb(29, 123, 222),
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
    OverlayMode overlay_mode{OverlayMode::Minimal};
    Engine const& m_eng; 
    SDL_Renderer* m_renderer;
    SDL_Window* m_window;
    f32 m_main_scale;

    glm::vec2 lg_screen_extent;
    auto init() -> void;
    auto update() -> void; // STUB
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


