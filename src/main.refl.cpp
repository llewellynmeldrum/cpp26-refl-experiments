#include <print>
#include "test_config.hpp"
#include "cfg_storage.hpp"
#include "cfg_toml_rw.hpp"
#include "cfg_metadata.refl.hpp"

int main(){


    cfg::validate_root_cfg<Config>();
    auto storage = cfg::Storage<Config>{};
    if (load_from_disk(storage)){
        std::println("Succesfully loaded config from storage:");
//        std::println("live: {}", storage);
    } else{
        std::println("Failed to load config from storage.");
    }
    return 0;
}
