#include <cpptrace/utils.hpp>
#include <format>
#include <ranges>
#include <print>

#include <libassert/assert.hpp>
#include <cpptrace/cpptrace.hpp>

#include "engine.hpp"
#include "cppslop.hpp"
#include "window.hpp"
#include "reflection_wrapper.hpp"

#include "reflect_formatter.refl.hpp"
#include "test_config.hpp"
#include "cfg_storage.hpp"
#include "cfg_toml_rw.hpp"
#include "cfg_metadata.refl.hpp"

#include <ranges>
#include <format>

USE_AUTO_FORMATTER(Vector2)
USE_AUTO_FORMATTER(MovementConfig)
USE_AUTO_FORMATTER(CameraConfig)
USE_AUTO_FORMATTER(DisplayConfig)
USE_AUTO_FORMATTER(PlayerConfig)
USE_AUTO_FORMATTER(Config)


int main(){
    cpptrace::register_terminate_handler();
    toml::color::enable();

    if (cfg::static_validate_root_cfg<Config>()){
        LOG_DBG("Validated config for T='{}'. No static errors.",std::string(std::meta::identifier_of(^^Config)));
    }
    auto storage = cfg::Storage<Config>{.filename = "config.toml"};
    //cfg::delete_config_file(storage);
    if (cfg::load_from_disk(storage)){
        LOG_INFO("Succesfully loaded config from storage ({}):",storage.filename);
        LOG_INFO("\nlive: {:t}", storage.live);
    } else{
        std::println("Failed to load config from storage.");
    }
    auto& e = Engine::get();
    while (!e.window.shouldClose){
        e.window.show();
    }
    return 0;
}
