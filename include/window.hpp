#pragma once 
#include <format>
#include <stdexcept>
#include <string>
#include <cstdio>

#include <SDL3/SDL.h>

#include "gui.hpp"
#include "primitive_types.hpp"
#include "glm/vec4.hpp"

struct Window{
    



    Window(std::string a_name, glm::ivec2 window_logical_extent);
    ~Window();
    auto begin_frame(glm::vec2 framebuffer_scale) ->void;
    auto draw_bullshit() ->void;
    auto present_frame() ->void;
    //auto show_ui() -> void;
    auto handle_inputs() -> void;

    auto close() -> void;
    bool shouldClose{false};

    std::string m_window_name;
    glm::ivec2 m_window_logical_extent;
    f32 m_main_scale;
    SDL_WindowFlags m_window_flags;
    SDL_Window* m_window;
    SDL_Renderer* m_renderer;

    auto clear(f32 r, f32 g, f32 b, f32 a) ->void;
    auto set_render_scale(f32 x, f32 y) ->void;
    glm::vec4 clear_color = {0.45f, 0.55f, 0.60f, 1.00f};
};
