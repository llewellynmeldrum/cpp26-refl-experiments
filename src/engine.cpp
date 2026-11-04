#include "engine.hpp"
#include "metrics.hpp"
void Engine::run(){
    while (!window.shouldClose){
        {
            auto _ = trk_input.tracker.push_scope_duration();
            window.handle_inputs();
        }
        {
            auto _ = trk_draw.tracker.push_scope_duration();
            window.begin_frame(gui.framebuffer_scale());
            window.draw_bullshit();
        }
        {
            auto _ = trk_gui.tracker.push_scope_duration();
            gui.begin_frame();
            gui.draw();
            gui.draw_config_window(config_storage);
            gui.end_frame();
        }
        { 
            auto _ = trk_present.tracker.push_scope_duration();
            window.present_frame();
        }
    }
}

