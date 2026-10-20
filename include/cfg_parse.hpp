
#pragma once 
#include <concepts>
#include <string>
#include <vector>
#include <ranges>

#include <toml11/parser.hpp>

#include "cfg_metadata.hpp"
#include "cfg_registration.hpp"
#include "file_io.hpp"

#include "cfg_parse.refl.hpp"
//#include "meta_helpers.refl.hpp"
#include "toml11/fwd/error_info_fwd.hpp"
#include "toml_traits.hpp"
#include "primitive_types.hpp"
#include "cppslop.hpp"

#if defined(CLANGD_PARSE)

template<typename T>
consteval auto _identifier_of(T const& val) -> std::string_view{
    return "UNKNOWN, NO REFLECTION"sv;
}

#else

template <typename E>
constexpr auto enum_to_string(E value) -> std::string_view{
    template for (constexpr auto e : std::define_static_array(std::meta::enumerators_of(^^E))) {
        if (value == [:e:]) { 
            return std::meta::identifier_of(e); 
        }
    }
    return "<unknown>";
}

#include <meta>
template<typename T> 
consteval auto _identifier_of(T const& v){
    if constexpr(std::is_enum_v<T>){
        return enum_to_string(v);
    }else{
        return "UNIMPLEMENTED";
    }
}
#endif

namespace cfg{

auto print_errors(std::span< const toml::error_info> errors){
    auto const err_count = errors.size();
    for (auto const& [idx,err]: std::views::enumerate(errors)){
        LOG_SUB_ERROR("Error {} of {}: {} ",idx+1, err_count, err.title());
        LOG_SUB_ERROR("-----------------------------");
        for (auto const& [loc, str]: err.locations()){
            LOG_SUB_ERROR("{}",toml::format_location(loc,str),str);
        }
        LOG_SUB_ERROR("{}",err.suffix());
    }
    //TODO: Currently, on parse error, we just return the previous value.
    // No changes are made.
}
// IN: file path to config file
// OUT: config struct of type T the parsed toml, empty on error.
template<typename T>
auto parse_toml(T const& current, fs::path config_file_path) -> T{
    LOG_DBG("Parsing '{}'...",config_file_path);
    auto input = toml::try_parse(config_file_path, toml::spec::v(1,1,0));
    if (input.is_err()){
        auto errors = input.unwrap_err();
        LOG_ERROR("Failed to parse toml file '{}', {} errors :",config_file_path,errors.size());
        print_errors(errors);
        return current;
    }
    LOG_DBG("Parsed '{}' succesfully. Reading values...", config_file_path);
    // reflect the fields, do the serialize shit but in reverse:
    auto errors = std::vector<toml::error_info>{};
    auto cur = current;
    read_into(cur, input.unwrap(), errors, "");
    if (errors.size() > 0){
        LOG_ERROR("Failed to load toml file '{}', {} errors :",config_file_path,errors.size());
        print_errors(errors);
    }else{
        LOG_DBG("Loaded '{}' succesfully, {} errors.",config_file_path,errors.size());
    }
    return cur;
}
}// NOTE: namespace cfg
