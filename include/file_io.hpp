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
inline auto save_to_file(fs::path file_path, std::span<const std::byte> bytes)->void{
    auto file = std::ofstream(file_path, std::ios::binary);
    if (!file){
        std::println("file error: unable to open '{}'",file_path);
        return;
    }
    auto const* data = reinterpret_cast<char const*>(bytes.data());
    file.write(data, bytes.size_bytes());
    if (!file){
        std::println("file error: unable to write to '{}'",file_path);
    }

}
inline auto delete_file(fs::path file_path)->void{
    try{
        if (fs::remove(file_path)){
            std::println(stderr, "Deleted file '{}' succesfully.",file_path.string());
        }else{
            std::println(stderr, "Unable to delete file, no error. ",file_path.string());
        }
    } catch (const fs::filesystem_error& e) {
        std::println(stderr, "Unable to delete file: {}",e.what());
    }
}
}// NOTE: namespace io
