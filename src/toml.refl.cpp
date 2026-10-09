#include <fstream>
#include <iostream>
#include <filesystem>


#include <toml++/toml.hpp>
#include <expected_fs/expected_fs.hpp>

#include "toml.refl.hpp"
#include "primitive_types.hpp"
#include "logger.hpp"

namespace fs = std::filesystem;
namespace efs = expected_fs;

template<typename T>
struct Registration;

struct ConfigStructRange{ f64 min, max; };
#ifdef CLANGD_PARSE
    #define RANGE(min,max) 
#else
    #define RANGE(min, max) [[=ConfigStructRange{min,max}]]
#endif

// in config.hpp
struct Config{
    RANGE(5.0f, 120.0f) f32 fov;
    RANGE(0.1f, 10.0f)  f32 move_speed;
};

namespace cfg{
template<>
struct Registration<Config>{
    using ConfigType = Config;
    static constexpr inline auto config_filename = "config.toml"sv; 
    static constexpr inline auto config_name = "Config"sv; 
};
template<typename T>
concept valid_registration = requires(T v){
    sizeof(Registration<T>); // check if the specialization exists at all

    {Registration<T>::config_file} -> std::same_as<std::string_view>;
    {Registration<T>::config_name} -> std::same_as<std::string_view>;

    requires Registration<T>::config_filename.ends_with(".toml");
}
// cfg: helpers
template<typename T> 
    requires valid_registration<T>
auto get_default() -> T { return T{}; }



template<typename T>
struct Storage{
    // TODO: This type should be special somehow, it should be impossible to
    // copy or move. The type itself doesnt hold a unique object (like a file),
    // but it holds the source of truth, which should be singular
    Storage(Storage const&) = delete;
    Storage(Storage &&) = delete;
    Storage& operator=(Storage const& ) = delete;
    Storage &operator=(Storage &&) = delete;

    T live{}; 
    std::optional<T> on_disk_cache {std::nullopt}; // the last read version on disk
};

template<typename T>
auto serialize_toml(T const& root) -> std::vector<std::byte>{
    // TODO: implement
    auto res = std::string{};
//    serialize_scalars_of
    return res;
}


template<typename T>
auto atomic_write_to_disk(fs::path config_file_path, T const& v) -> void {
    // this should probably assert that the real file exists,
    // and warn if the tmp file lingered from a previous run. 
    // we do atomic writes to a scratch file to preserve previous config files in the case of a crash mid-write
    // Optionally, we might wish to dump the live config in the case of a crash (those handled gracefully, at least)

    auto bytes = cfg::serialize_toml<T>(storage.live);
    auto tmp_filepath = fs::path(config_file_path.string() + ".tmp";);
    {
        auto tmp_file = std::ofstream(tmp_filepath);
        io::save_to_file(tmp_file, bytes);
    }

    // i.e, move file
    fs::rename(tmp_filepath, config_file_path);
}

} // NOTE: namespace cfg


// BRIEF:
// Saves the currently live config in storage to disk, 
// and updates the disk cache to match.
// live -> disk
template<typename T> 
    requires valid_registration<T>
auto save_to_disk(cfg::Storage<T>& storage) -> void{
    // if disk copy exists but no live:
    // if live copy exists but no disk -> save to disk, warn
    using Reg = cfg::Registration<T>;

    auto const config_file_path = fs::path(Reg::config_file);
    // QUESTION: should save really care whether or not the config file exists?
    // This shouldnt really be a warning; more of an LOG_INFO
    if (!fs::exists(config_file_path)){
        LOG_WARN(
            "Whilst trying to WRITE config file '{}' to disk:\n"
            "No config file ('{}') exists for registered config struct '{}'. \n"
            "Creating file '{}' with defaults from '{}'...",
            Reg::config_file,
            Reg::config_file, Reg::config_name,
            Reg::config_file, Reg::config_name
        );
        io::create_file(config_file_path);
        cfg::atomic_write_to_disk<T>(config_file_path, cfg::get_default());
    }


    cfg::atomic_write_to_disk<T>(config_file_path, storage.live);
    // disk now reflects live storage. live, update the cache 
    storage.on_disk_cache = storage.live; 
}

// BRIEF:
// Loads the config on disk into disk cache, and copies into live.
// disk -> live
template<typename T>
auto load_from_disk<T>(cfg::Storage<T>& storage, fs::path config_file_path) -> T{
    using Reg = cfg::Registration<T>;

    auto const config_file_path = fs::path(Reg::config_file);
    if (!fs::exists(config_file_path)){
        LOG_WARN(
            "Whilst trying to LOAD config file '{}' from disk:\n"
            "No config file ('{}') exists for registered config struct '{}'. \n"
            "Creating file '{}' with defaults from '{}'...",
            Reg::config_file,
            Reg::config_file, Reg::config_name,
            Reg::config_file, Reg::config_name
        );
        io::create_file(config_file_path);
        cfg::atomic_write_to_disk<T>(config_file_path, cfg::get_default());
    }
    storage.on_disk_cache = cfg::deserialize_toml<T>(config_file_path);
    storage.live = *storage.on_disk_cache;
}



