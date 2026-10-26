#pragma once 
#include "logger.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>
#include <vector>

namespace fs = std::filesystem;
namespace io{
inline auto to_byte_vector(std::string_view s) -> std::vector<std::byte> {

    return std::vector<std::byte>(
        std::from_range,
        std::as_bytes(
            std::span(s)
        )
    );
}
inline auto create_file(fs::path file_path) -> void {
    if (fs::exists(file_path)){
        LOG_WARN("called create_file({}) when '{}' already exists.",file_path,file_path);
    }
    auto file = std::ofstream(file_path, std::ios::binary);
    if (!file){
        std::println("file error: unable to create file: '{}'",file_path);
        return;
    }
}
inline auto save_to_file(fs::path file_path, std::span<const std::byte> bytes)->bool{
    auto file = std::ofstream(file_path, std::ios::binary);
    if (!file){
        std::println("file error: unable to open '{}'",file_path);
        return false;
    }
    auto const* data = reinterpret_cast<char const*>(bytes.data());
    file.write(data, bytes.size_bytes());
    if (!file){
        std::println("file error: unable to write to '{}'",file_path);
        return false;
    }
    return true;

}
inline auto report_error(std::string sv, std::error_code const& err){
    LOG_ERROR("{} .\nDetails: {} -> {}",sv,err.category().name(), err.message());
}
inline auto move_file_to(fs::path src, fs::path dst)->bool{
    auto err = std::error_code{};
    fs::rename(src, dst,err);
    if (err){
        report_error(std::format("Unable to move file '{}' to '{}'",src.string(), dst.string()), err);
        return false;
    }
    LOG_DBG("Successfully moved file '{}' to '{}'"sv,src.string(),dst.string());
    return true;
}
inline auto delete_file(fs::path file_path)->bool{
    auto err = std::error_code{};
    fs::remove(file_path, err);
    if (err){
        report_error(std::format("Unable to delete file '{}'.",file_path.string()), err);
        return false;
    }
    LOG_DBG("Deleted file '{}' succesfully."sv,file_path.string());
    return true;
}

inline auto atomic_save_to_file(fs::path filepath, std::span<const std::byte>bytes) -> bool {
    // we do atomic writes to a scratch file to preserve previous config files in the case of a crash mid-write

    auto const tmp_filepath = fs::path{filepath.string() + ".tmp"};
    {
        if (!io::save_to_file(tmp_filepath, bytes)){
            LOG_ERROR("{}(): Unable to save to temporary filepath",__FUNCTION__);
            return false;
        }
    }
    if (!io::move_file_to(tmp_filepath, filepath)){
        LOG_ERROR("{}(): Unable to move temp file to true file. ",__FUNCTION__);
            return false;
    }
    return true;
}
}// NOTE: namespace io
