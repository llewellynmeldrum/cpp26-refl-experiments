#include "window.hpp"
#include <imgui.h>

auto Window::close() -> void{
    shouldClose = true;
}
Window::Window(std::string a_name)
    :m_window_name(a_name)
{
    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD))
    {
        throw std::runtime_error(
            std::format( "Unable to init SDL ({}() failed): {} ",
                "SDL_Init", SDL_GetError()
            )
        );
    }

    // Create window with SDL_Renderer graphics context
    m_main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    m_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    m_window = SDL_CreateWindow(m_window_name.c_str(), (int)(1280 * m_main_scale), (int)(800 * m_main_scale), m_window_flags);
    if (!m_window) {
        throw std::runtime_error(
            std::format( "Unable to init SDL ({}() failed): {} ",
                "SDL_CreateWindow", SDL_GetError()
            )
        );
    }
    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    //SDL_SetRenderVSync(renderer, 1);
    if (m_renderer == nullptr) {
        throw std::runtime_error(
            std::format( "Unable to init SDL ({}() failed): {} ",
                "SDL_CreateRenderer", SDL_GetError()
            )
        );
    }
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    ui.init(m_renderer,m_window,m_main_scale);

    SDL_SetHint(SDL_HINT_FORCE_RAISEWINDOW, "1");
    SDL_ShowWindow(m_window);
    SDL_RaiseWindow(m_window);
}
auto Window::clear(f32 r, f32 g, f32 b, f32 a) -> void{
    SDL_SetRenderDrawColorFloat(m_renderer, r,g,b,a);
    SDL_RenderClear(m_renderer);
}
auto Window::begin_frame() ->void{
    if (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED) {
        SDL_Delay(10);
        return;
    }
    set_render_scale(ui.framebuffer_scale_x(), ui.framebuffer_scale_y());
    clear(ui.clear_color.x, ui.clear_color.y, ui.clear_color.z, ui.clear_color.w);
}
auto Window::present_frame() ->void{
    SDL_RenderPresent(m_renderer);
}
auto Window::set_render_scale(f32 x, f32 y) ->void{
    SDL_SetRenderScale(m_renderer, x, y); 
}
auto Window::show() ->void{
    handle_inputs();
    begin_frame();
    ui.show();
    present_frame();
}

auto Window::handle_inputs() -> void{
    SDL_Event event;
    auto handle_key_down = [&](auto key, auto mods){
        if (mods & SDL_KMOD_CTRL){
            if (key == SDLK_C){
                shouldClose = true;
            }
        }
    };

    while (SDL_PollEvent(&event)) {
        ImGui_ImplSDL3_ProcessEvent(&event);
        if (event.type == SDL_EVENT_QUIT){
            shouldClose = true;
        }
        if (ig::GetIO().WantCaptureKeyboard){ continue; }

        if (event.type == SDL_EVENT_KEY_DOWN){
            handle_key_down(event.key.key, event.key.mod);
        }
        if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED && event.window.windowID == SDL_GetWindowID(m_window)){
            shouldClose = true;
        }
    }
}

Window::~Window(){
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();

    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
