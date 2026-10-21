#include "gui.hpp"
#include "engine.hpp"
#include "gui_handler.refl.hpp"

auto Gui::edit_configs() -> void{
    auto& e = Engine::get();
    bool edited = cfg::edit_config_window(e.config);
    if (edited){
        LOG_DBG("config edited");
    }
}
