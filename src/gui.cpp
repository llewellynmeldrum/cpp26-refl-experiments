#include <format>

#include "gui.hpp"
#include "cfg_metadata.hpp"
#include "cfg_registration.hpp"
#include "engine.hpp"
#include "imgui_wrapper.hpp"
#include "common.hpp"
#include "gui_handler.hpp"
struct GuiSettings{
    f32 font_size = 20.0f;

};
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
    ImGuiIO& io = ig::GetIO(); 
    io.IniFilename = nullptr; 
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

    // Setup Dear ImGui style
    ImGuiStyle& style = ig::GetStyle();
    ig::StyleColorsDark();
    style.FontSizeBase = 16.0f;
    io.Fonts->AddFontDefaultVector();
    style.FrameRounding = 6.0f;
    style.GrabRounding = 6.0f;

    // Setup scaling
    style.ScaleAllSizes(main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

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


#if CPP26

auto Gui::edit_configs() -> void{
    auto& e = Engine::get();
    using T = Config;
//    if (!cfg::is_declaration_valid<Enemy,^^Enemy::name>({})){
//        ASSERT(false, "bruh");
//    }
//    cfg::static_validate_root_cfg<T>();

    auto res = cfg::edit_config_window(e.config_storage);
    if (res.reset_to_default){
        e.config_storage.live = {};
    }else if (res.save_to_disk){
        cfg::save_to_disk(e.config_storage);
    }else if (res.load_from_disk){
        cfg::load_from_disk(e.config_storage);
    }
    if (res.edited){
        LOG_DBG("config edited"sv);
    }
}
#endif // CPP26
