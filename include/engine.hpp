#pragma once 
#include "cfg_storage.hpp"
#include "gui.hpp"
#include "metrics.hpp"
#include "window.hpp"
#include <map>
#if PERMIT_ENGINE_HPP_INCLUDE
// This is to ensure only specific files include engine.hpp.
// If we let anyone include the root struct, i've found it causes many problems down the line.
// The files that can include it currently are the debug ui and the 
#else 
#error "This file has not been approved to #include engine.hpp! If this is a mistake, add it in CMakeLists.txt"
#endif

#include "test_config.hpp"

struct Engine{
    NO_MOVE_CTOR(Engine)
    NO_MOVE_ASSN(Engine)
    NO_COPY_CTOR(Engine)
    NO_COPY_ASSN(Engine)

    Engine() 
        : window ("Test window", glm::ivec2{1600,900}) 
        , gui(*this, window.m_renderer, window.m_window, window.m_main_scale)
    {}
    ~Engine()=default;
    static auto tracker_stages() -> std::vector<TrackerStage>{
        return {trk_input,trk_draw,trk_gui,trk_present};
    };
    static auto tracker_frametime() -> TrackerStage const&{
        return trk_frametime;
    }

    void run();
    Window window;
    Gui gui;
    cfg::Storage<Config> config_storage{.filename = "cfg.toml"sv};


};
