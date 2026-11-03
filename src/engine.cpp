#include "engine.hpp"
void Engine::run(){
    while (!window.shouldClose){
        auto _ = ms_frametime.sample_scope_duration();
        {
            // input time
            window.handle_inputs();
        }
        {
            // draw time
            window.begin_frame(gui.framebuffer_scale());
            window.draw_bullshit();
        }
        {
            // gui time
            gui.begin_frame();
            gui.draw();
            gui.draw_config_window(config_storage);
            gui.end_frame();
        }
        { // present time
            window.present_frame();
        }
    }
}

