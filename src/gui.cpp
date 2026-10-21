#include "gui.hpp"
#include "engine.hpp"
#include <format>
auto Gui::init(
    SDL_Renderer* renderer,
    SDL_Window* window,
    f32 main_scale
) -> void{
    m_renderer = renderer;
    m_window = window;
    m_main_scale = main_scale;
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ig::CreateContext();
    ImGuiIO& io = ig::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking

    // Setup Dear ImGui style
    ImGuiStyle& style = ig::GetStyle();
    ig::StyleColorsDark();
    //ig::StyleColorsLight();
    //style.FontSizeBase = 16.0f;           // Default to 13.0f when unspecified, which allows crisp legacy font for non-4K screens.
    style.FrameRounding = 3.0f;
    style.GrabRounding = 3.0f;

    // Setup scaling
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)
    //io.ConfigDpiScaleFonts = true;        // [Experimental] Automatically overwrite style.FontScaleDpi in Begin() when Monitor DPI changes. This will scale fonts but _NOT_ scale sizes/padding for now.
    //io.ConfigDpiScaleViewports = true;    // [Experimental] Scale Dear ImGui and Platform Windows when Monitor DPI changes.

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
    ImGui_ImplSDLRenderer3_Init(renderer);


}


auto Gui::begin_frame() -> void{
    // Start the Dear ImGui frame
    ImGui_ImplSDLRenderer3_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ig::NewFrame();
}
auto Gui::end_frame() -> void{
    ig::Render();
    ImGui_ImplSDLRenderer3_RenderDrawData(ig::GetDrawData(), m_renderer);
}

auto Gui::show() -> void{
    auto io = ig::GetIO();
    begin_frame();
    edit_configs();
    end_frame();
}

auto Gui::framebuffer_scale_x() -> f32{
    return ig::GetIO().DisplayFramebufferScale.x;
}
auto Gui::framebuffer_scale_y() -> f32{
    return ig::GetIO().DisplayFramebufferScale.y;
}
