#include <format>
#include <implot.h>

#include "gui.hpp"
#include "cfg_metadata.hpp"
#include "cfg_registration.hpp"
#include "cfg_storage.hpp"
#include "engine.hpp"
#include "imgui_wrapper.hpp"
#include "common.hpp"
#include "cfg_gui.hpp"
#include "gui_metrics.hpp"
#include "engine.hpp"
#include "lmgui_general.hpp"
#include "lmgui_windows.hpp"

struct GuiSettings{
    f32 font_size = 20.0f;

};
auto Gui::update() -> void{
}
auto Gui::init() -> void{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ig::CreateContext();
    ImPlot::CreateContext();

    ImGuiIO& io = ig::GetIO(); 
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
    style.ScaleAllSizes(m_main_scale);        // Bake a fixed style scale. (until we have a solution for dynamic style scaling, changing this requires resetting Style + calling this again)
    style.FontScaleDpi = m_main_scale;        // Set initial font scale. (in docking branch: using io.ConfigDpiScaleFonts=true automatically overrides this for every window depending on the current monitor)

    // Setup Platform/Renderer backends
    ImGui_ImplSDL3_InitForSDLRenderer(m_window, m_renderer);
    ImGui_ImplSDLRenderer3_Init(m_renderer);


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


auto draw_frametime_overlay(
    TrackerStage frametime_tracker,
    std::vector<TrackerStage> stages_tracker,
    OverlayMode& overlay_mode
) -> void{
    auto io = ig::GetIO();
    auto spec0 = ui::WindowSpec{"Frametime"} 
        .disable_docking()
        #if CPP26
        .set_flags(ui::WindowFlagBits::eNone)
        #endif
    ;
    if (overlay_mode == OverlayMode::Hidden|| overlay_mode == OverlayMode::Minimal || overlay_mode == OverlayMode::BreakdownOnly){
        spec0.add_flags(ui::WindowFlagBits::eNoBackground);
    }
    if(auto window = ui::begin_scope(spec0)){
        ui::EnumDropdown("Overlay Mode", overlay_mode);
        GuiDrawContext dctx{
            .overlay_mode = overlay_mode
        };
        auto stats = HistoryBufferStats(frametime_tracker.tracker.hbuf);
        auto const title_w_stats = stats.summary();
        if (overlay_mode == OverlayMode::Full){
            draw_frametime_graph(stats,frametime_tracker, dctx, title_w_stats);
            draw_stacked_frametime_breakdown_graph(stages_tracker, dctx, "##");
        }else if (overlay_mode ==OverlayMode::BreakdownOnly){
            draw_stacked_frametime_breakdown_graph(stages_tracker, dctx, title_w_stats);
        }else if (overlay_mode ==OverlayMode::Minimal){
            sparkline(frametime_tracker, {"frametime", 1.0f, stats});
        }else if (overlay_mode == OverlayMode::Hidden){
            // nothing
        }
    }

}
auto Gui::draw() -> void{
    draw_frametime_overlay(m_eng.tracker_frametime(), m_eng.tracker_stages(),overlay_mode);
    m_frame_count++;
}

auto Gui::framebuffer_scale() -> glm::vec2{
    return {ig::GetIO().DisplayFramebufferScale.x,ig::GetIO().DisplayFramebufferScale.y};
}
auto Gui::framebuffer_scale_x() -> f32{
    return ig::GetIO().DisplayFramebufferScale.x;
}
auto Gui::framebuffer_scale_y() -> f32{
    return ig::GetIO().DisplayFramebufferScale.y;
}



auto Gui::shutdown() -> void{
    ImGui_ImplSDLRenderer3_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImPlot::DestroyContext();
    ImGui::DestroyContext();
}
