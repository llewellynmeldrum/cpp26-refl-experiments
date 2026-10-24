#include <cpptrace/utils.hpp>
#include <format>
#include <ranges>
#include <print>

#include <libassert/assert.hpp>
#include <cpptrace/cpptrace.hpp>

#include "engine.hpp"
#include "cppslop.hpp"
#include "window.hpp"
#include "common.hpp"

#include "test_config.hpp"
#include "cfg_storage.hpp"
#include "cfg_toml_rw.hpp"
#include "cfg_metadata.hpp"
#include "auto_format.hpp"

int main(){
    cpptrace::register_terminate_handler();
    toml::color::enable();

    auto str = std::string{};
    #if CPP26
    str = std::string(std::meta::identifier_of(^^Config));
    #endif 
    auto storage = cfg::Storage<Config>{.filename = "config.toml"};
    if (cfg::load_from_disk(storage)){
        LOG_INFO("Loaded config from '{}':",storage.filename);
        IF_CPP26(
            LOG_INFO("\nlive: {:t}", storage.live);
        )
    } else{
        LOG_ERROR("Failed to load config from storage.");
    }
    auto& e = Engine::get();
    while (!e.window.shouldClose){
        e.window.show();
    }
    return 0;
}
