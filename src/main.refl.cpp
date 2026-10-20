#include <format>
#include <ranges>
#include <print>

#include <libassert/assert.hpp>
#include <cpptrace/cpptrace.hpp>

#include "reflect_formatter.refl.hpp"
#include "test_config.hpp"
#include "cfg_storage.hpp"
#include "cfg_toml_rw.hpp"
#include "cfg_metadata.refl.hpp"
#include "cppslop.hpp"
#include "window.hpp"

template <> struct std::formatter<Vector2> : refl::auto_formatter {};
USE_AUTO_FORMATTER(MovementConfig)
USE_AUTO_FORMATTER(CameraConfig)
USE_AUTO_FORMATTER(DisplayConfig)
USE_AUTO_FORMATTER(PlayerConfig)
USE_AUTO_FORMATTER(Config)

int main(){
    if (cfg::validate_root_cfg<Config>()){
        LOG_DBG("Validated config for T='{}'. No static errors.",std::string(std::meta::identifier_of(^^Config)));
    }
    auto storage = cfg::Storage<Config>{.filename = "bad.toml"};
//    cfg::delete_config_file(storage);
//    if (cfg::load_from_disk(storage)){
//        LOG_INFO("Succesfully loaded config from storage ({}):",storage.filename);
//        LOG_INFO("\nlive: {}", storage.live);
//    } else{
//        std::println("Failed to load config from storage.");
//    }
    auto window = Window{"Test window"};
    while (!window.shouldClose){
        window.show();
    }
    return 0;
}
