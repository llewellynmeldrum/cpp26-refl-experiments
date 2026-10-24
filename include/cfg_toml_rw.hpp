#pragma once
#include "cfg_helpers.hpp"
#include "cfg_metadata.hpp"
#include "cfg_parse.hpp"
#include "cfg_storage.hpp"
#include "file_io.hpp"
#include "cfg_serialize.hpp"

namespace cfg{

template<typename T>
auto write_to_file(fs::path config_file_path, T const& v) -> bool {
    return io::atomic_save_to_file(config_file_path, cfg::serialize_toml<T>(v));
}

template<typename T> 
    requires cfg::is_root_config<T>
auto delete_config_file(cfg::Storage<T>& storage) -> bool{
    // if disk copy exists but no live:
    // if live copy exists but no disk -> save to disk, warn
    return io::delete_file(storage.filename);
}
template<typename T> 
    requires cfg::is_root_config<T>
auto save_to_disk(cfg::Storage<T>& storage) -> void{
    // if disk copy exists but no live:
    // if live copy exists but no disk -> save to disk, warn
    auto reg = cfg::get_registration<T>();

    auto const config_file_path = fs::path(storage.filename);
    if (!fs::exists(config_file_path)){
        LOG_INFO(
            "Whilst trying to SAVE config file '{}' to disk:\n"
            "No config file ('{}') exists for registered config struct '{}'. \n"
            "Creating file '{}' with defaults from '{}'...",
            config_file_path,
            config_file_path, reg.config_name,
            config_file_path, reg.config_name
        );
        io::create_file(config_file_path);
        cfg::write_to_file<T>(config_file_path, T{});
    }

    validate_root_cfg<T>(storage.live);

    cfg::write_to_file<T>(config_file_path, storage.live);
    // disk now reflects live storage. live, update the cache 
    storage.on_disk_cache = storage.live; 
}

// BRIEF:
// Loads the config on disk into disk cache, and copies into live.
// disk -> live
// returns true on success, false on failure
template<typename T>
auto load_from_disk(cfg::Storage<T>& storage) -> bool{
    auto reg = cfg::get_registration<T>();

    auto const config_file_path = fs::path(storage.filename);
    if (!fs::exists(config_file_path)){
        LOG_WARN(
            "Whilst trying to LOAD config file '{}' from disk:\n"
            "\tNo config file ('{}') exists for registered config struct '{}'. \n"
            "\tCreating file '{}' with defaults from '{}'...",
            config_file_path,
            config_file_path, reg.config_name,
            config_file_path, reg.config_name
        );
        cfg::write_to_file<T>(config_file_path, T{});
    }
    storage.on_disk_cache = cfg::parse_toml<T>(storage.live, config_file_path);
    storage.live = *storage.on_disk_cache;
    return true;
}

} //NOTE: namespace cfg
