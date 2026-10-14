#pragma once
#include "cfg_helpers.hpp"
#include "cfg_storage.hpp"
#include "file_io.hpp"
// BRIEF:
// Saves the currently live config in storage to disk, 
// and updates the disk cache to match.
// live -> disk

namespace cfg{
template<typename T>
auto serialize_toml(T const& root) -> std::vector<std::byte>{
    auto res = std::string{};
    // TODO: implement writer. On failure,
//    serialize_scalars_of
    return io::to_byte_vector(res);
}

// IN: file path to config file
// OUT: config struct of type T the parsed toml, empty on error.
template<typename T>
auto parse_toml(fs::path config_file_path) -> std::optional<T>{
    auto res = std::optional<T>(cfg::get_default<T>());
    // TODO: implement parsing. On failure, return std::nullopt.
    return res;
}
template<typename T>
auto write_to_file(fs::path config_file_path, T const& v) -> void {
    // this should probably assert that the real file exists,
    // and warn if the tmp file lingered from a previous run. 
    // we do atomic writes to a scratch file to preserve previous config files in the case of a crash mid-write
    // Optionally, we might wish to dump the live config in the case of a crash (those handled gracefully, at least)
    io::atomic_save_to_file(config_file_path, cfg::serialize_toml<T>(v));
}

template<typename T> 
    requires cfg::valid_registration<T>
auto save_to_disk(cfg::Storage<T>& storage) -> void{
    // if disk copy exists but no live:
    // if live copy exists but no disk -> save to disk, warn
    using Reg = cfg::Registration<T>;

    auto const config_file_path = fs::path(Reg::config_filename);
    // QUESTION: should save really care whether or not the config file exists?
    // This shouldnt really be a warning; more of an LOG_INFO
    if (!fs::exists(config_file_path)){
        LOG_WARN(
            "Whilst trying to WRITE config file '{}' to disk:\n"
            "No config file ('{}') exists for registered config struct '{}'. \n"
            "Creating file '{}' with defaults from '{}'...",
            Reg::config_filename,
            Reg::config_filename, Reg::config_name,
            Reg::config_filename, Reg::config_name
        );
        io::create_file(config_file_path);
        cfg::write_to_file<T>(config_file_path, cfg::get_default<T>());
    }


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
    using Reg = cfg::Registration<T>;

    auto const config_file_path = fs::path(Reg::config_filename);
    if (!fs::exists(config_file_path)){
        LOG_WARN(
            "Whilst trying to LOAD config file '{}' from disk:\n"
            "No config file ('{}') exists for registered config struct '{}'. \n"
            "Creating file '{}' with defaults from '{}'...",
            Reg::config_filename,
            Reg::config_filename, Reg::config_name,
            Reg::config_filename, Reg::config_name
        );
        io::create_file(config_file_path);
        cfg::write_to_file<T>(config_file_path, cfg::get_default<T>());
    }
    auto res = cfg::parse_toml<T>(config_file_path);
    if (!res){
        return false;
    }
    storage.on_disk_cache = *res;
    storage.live = *storage.on_disk_cache;
    return true;
}

} //NOTE: namespace cfg
