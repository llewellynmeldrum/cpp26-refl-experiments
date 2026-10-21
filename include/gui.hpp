#pragma once 

#include <SDL3/SDL.h>

#include "imgui_wrapper.hpp"
#include "cfg_helpers.hpp"

struct Gui{
    SDL_Renderer* m_renderer;
    SDL_Window* m_window;
    f32 m_main_scale;

    auto init(
        SDL_Renderer* renderer,
        SDL_Window* window,
        f32 main_scale
    ) -> void;
    auto show() -> void;
    auto edit_configs() -> void;



    auto framebuffer_scale_x() -> f32;
    auto framebuffer_scale_y() -> f32;

    bool show_demo_window = true;
    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

private:
    auto begin_frame() -> void;
    auto end_frame() -> void;
};


