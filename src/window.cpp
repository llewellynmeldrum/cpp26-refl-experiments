#include "window.hpp"
#include <imgui.h>
#include <libassert/assert.hpp>

auto Window::close() -> void{
    shouldClose = true;
}
Window::Window(std::string a_name, glm::ivec2 window_logical_extent)
    :m_window_name(a_name)
    ,m_window_logical_extent(window_logical_extent)
{

    ASSERT(
        SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD), 
        std::format( "Unable to init SDL ({}() failed): {} ", "SDL_Init", SDL_GetError())
    );

    // Create window with SDL_Renderer graphics context
    m_main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
    m_window_flags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
    auto window_pixel_extent = glm::ivec2{glm::vec2{window_logical_extent} * m_main_scale};
    m_window = SDL_CreateWindow(
        m_window_name.c_str(),
        window_pixel_extent.x,
        window_pixel_extent.y,
        m_window_flags
    );

    ASSERT(
        m_window,
        std::format( "Unable to init SDL ({}() failed): {} ", "SDL_CreateWindow", SDL_GetError())
    );

    m_renderer = SDL_CreateRenderer(m_window, nullptr);
    //SDL_SetRenderVSync(renderer, 1);
    ASSERT(
        m_renderer,
        std::format( "Unable to init SDL ({}() failed): {} ", "SDL_CreateRenderer", SDL_GetError())
    );
    SDL_SetWindowPosition(m_window, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED);

    SDL_SetHint(SDL_HINT_FORCE_RAISEWINDOW, "1");
    SDL_ShowWindow(m_window);
    SDL_RaiseWindow(m_window);
}
auto Window::clear(f32 r, f32 g, f32 b, f32 a) -> void{
    SDL_SetRenderDrawColorFloat(m_renderer, r,g,b,a);
    SDL_RenderClear(m_renderer);
}
auto Window::draw_bullshit() ->void{
    std::srand(time(NULL));
    auto random = [](f32 min, f32 max){
        return min + ((std::rand() / (f32)RAND_MAX) * (max-min));
    };
    auto c = random(0,1);
    SDL_SetRenderDrawColorFloat(m_renderer, c,c,c,1.0f);
    for (int i = 0; i<100000; i++){
        auto x = random(0,m_window_logical_extent.x);
        auto y = random(0,m_window_logical_extent.y);
        SDL_RenderPoint(m_renderer, x,y);
    }
}
auto Window::begin_frame(glm::vec2 framebuffer_scale) ->void{
    if (SDL_GetWindowFlags(m_window) & SDL_WINDOW_MINIMIZED) {
        SDL_Delay(10);
        return;
    }
    set_render_scale(framebuffer_scale.x,framebuffer_scale.y);
    clear(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
}
auto Window::present_frame() ->void{
    SDL_RenderPresent(m_renderer);
}
auto Window::set_render_scale(f32 x, f32 y) ->void{
    SDL_SetRenderScale(m_renderer, x, y); 
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
    SDL_DestroyRenderer(m_renderer);
    SDL_DestroyWindow(m_window);
    SDL_Quit();
}
