#pragma once 
#include <format>
#include <stdexcept>
#include <string>
#include <cstdio>

#include <SDL3/SDL.h>

#include "gui.hpp"
#include "primitive_types.hpp"

struct Window{
    Window(std::string a_name);
    ~Window();
    Gui ui;
    auto show() ->void;
    //auto show_ui() -> void;
    auto handle_inputs() -> void;

    auto close() -> void;
    bool shouldClose{false};

    std::string m_window_name;
    f32 m_main_scale;
    SDL_WindowFlags m_window_flags;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    auto clear(f32 r, f32 g, f32 b, f32 a) ->void;
    auto present_frame() ->void;
    auto begin_frame() ->void;
    auto set_render_scale(f32 x, f32 y) ->void;
};
