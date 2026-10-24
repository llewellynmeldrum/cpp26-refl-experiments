
#pragma once 
#include <concepts>
#include <string>
#include <vector>
#include <ranges>

#include <toml11/parser.hpp>

#include "cfg_metadata.hpp"
#include "cfg_registration.hpp"
#include "file_io.hpp"

#include "common.hpp"
#if CPP26
// --- formerly cfg_parse.refl.hpp ---
#include <ranges>
#include <vector>
#include <string_view>
#include <expected>
#include <string>
#include <meta>

#include <toml11/parser.hpp>

#include "cfg_helpers.hpp"
#include "cfg_traits.hpp"

namespace cfg{
template<typename ExpectedType>
auto check_type_mismatch(
    toml::value const& v,
    std::string_view path
) -> std::optional<toml::error_info>{
    auto bad_match = [&v, &path](std::string_view expect_type, std::string_view bad_type){
        return toml::make_error_info(
            "[Loading Error]",
            v.location(),
            std::format(
                "Field '{}' expects literal compatible with type '{}',"
                "but config contains a value of type '{}'. Field is unchanged/defaulted.",
                path,expect_type,bad_type
            ) 
        );
    };
    if constexpr (cfg::concepts::is_scalar_boolean<ExpectedType>){
        if (!v.is_boolean()){
            return bad_match(stable_type_name<ExpectedType>(), toml::to_string(v.type()));
        }
    }else if constexpr(cfg::concepts::is_scalar_floating_point<ExpectedType>){
        if (!v.is_floating()){
            return bad_match(stable_type_name<ExpectedType>(), toml::to_string(v.type()));
        }
    }else if constexpr(cfg::concepts::is_string_type<ExpectedType>){
        if (!v.is_string()){
            return bad_match(stable_type_name<ExpectedType>(), toml::to_string(v.type()));
        }
    }else if constexpr(cfg::concepts::is_scalar_char<ExpectedType>){
        if (!v.is_string()){
            return bad_match(stable_type_name<ExpectedType>(), toml::to_string(v.type()));
        }
    }else if constexpr(cfg::concepts::is_scalar_integral<ExpectedType>){
        if (!v.is_integer()){
            return bad_match(stable_type_name<ExpectedType>(), toml::to_string(v.type()));
        }
    }
    return std::nullopt;
}
inline auto toml_val_is_scalar(toml::value const& v) -> bool{
    if (
       v.type() == toml::value_t::boolean 
       || v.type() == toml::value_t::integer
       || v.type() == toml::value_t::floating
       || v.type() == toml::value_t::string
    ){
        return true;
    }else{
        return false;
    }
}
template<typename T>
auto read_scalar(
    toml::value const& v,
    std::string_view path
) -> std::expected<T, toml::error_info>{
    auto get_error = [&path, &v](std::string const&  msg) -> std::unexpected<toml::error_info>{
        return std::unexpected(toml::make_error_info(
            "[Invalid Value Type]",
            v.location(),
           std::format("Field '{}' {}. Field is unchanged/defaulted.",path, msg)
        ));
    };
    using U = nocvref<T>;
    if (!toml_val_is_scalar(v)){
        return get_error(std::format(
            "expects scalar type, but config contains type '{}'",
            toml::to_string(v.type())
        ));
    }
    auto type_match_error = check_type_mismatch<U>(v,path);
    if (type_match_error){
        return std::unexpected(*type_match_error);
    }

    // NOTE: 
    // From here, we can assume the types are at least of the correct category.
    if constexpr (cfg::concepts::is_scalar_boolean<T>){
        return v.as_boolean();
    } else if constexpr(cfg::concepts::is_scalar_floating_point<T>){
        auto as_f64 = v.as_floating();
        if constexpr (std::same_as<U, f32>){
            static constexpr auto f32_max = std::numeric_limits<f32>::max();
            if (as_f64 > f32_max){
                return get_error(std::format(
                    "expects an f32 compatible value, but config contains a value exceeding f32_max ({}).",
                    f32_max
                ));
            }
        }
        return static_cast<U>(as_f64);
    }else if constexpr(cfg::concepts::is_string_type<T>){
        auto as_str = v.as_string();
        return as_str;
    }else if constexpr(cfg::concepts::is_scalar_char<T>){
        auto as_str = v.as_string();
        if (as_str.length() > 1){
            return get_error(std::format(
                "expects a char field, but config contains a string of >1 ({}) chars.",
                as_str.length()
            ));
        }
        return as_str[0];
    }else if constexpr(cfg::concepts::is_scalar_integral<T>){
        auto i64_val = v.as_integer();
        if constexpr (std::same_as<U, i32>){
            static constexpr auto i32_min = std::numeric_limits<i32>::lowest();
            static constexpr auto i32_max = std::numeric_limits<i32>::max();
            if (i64_val < i32_min){
                return get_error(std::format(
                    "expects an i32 compatible value, but config contains a value less than i32_min ({}).",
                    i32_min
                ));
            }
            if (i64_val > i32_max){
                return get_error(std::format(
                    "expects an i32 compatible value, but config contains a value exceeding i32_max ({}).",
                    i32_max
                ));
            }
        }else if constexpr (std::same_as<U, u32>){
            static constexpr auto u32_min = std::numeric_limits<u32>::lowest();
            static constexpr auto u32_max = std::numeric_limits<u32>::max();
            if (i64_val < u32_min){
                return get_error(std::format(
                    "expects an u32 compatible value, but config contains a value less than u32_min ({}).",
                    u32_min
                ));
            }
            if (i64_val > u32_max){
                return get_error(std::format(
                    "expects an u32 compatible value, but config contains a value exceeding u32_max ({}).",
                    u32_max
                ));
            }
        }
        return static_cast<U>(i64_val);
    }
}

template<typename T>
auto read_into(
    T & out,
    toml::value const& v,
    std::vector<toml::error_info>& errors,
    std::string path
) -> void;


template<typename T>
auto read_array(
    T & out,
    toml::value const& v,
    std::vector<toml::error_info>& errors,
    std::string path
) -> void{
    auto report_error = [&errors, &path, &v](std::string const&  msg){
        errors.push_back(toml::make_error_info(
            "[Loading Error]",
            v.location(),
           std::format("Field '{}' {}. Field is unchanged/defaulted.",path, msg) 
        ));
    };
    if (!v.is_array()){
        report_error(std::format(
            "expects array type, but config contains type '{}'",
            toml::to_string(v.type())
        ));
        return;
    }
    auto as_array = v.as_array();
    if constexpr (traits::is_bounded_array_type<T>){
        auto const expected_size = out.size();
        if (as_array.size() != expected_size){
            report_error(std::format(
                "has a size of {}, but config contains array of size {}",
                as_array.size(), expected_size
            ));
            return;
        }
    }

    toml::value_t first_type = {};
    auto idx = 0uz;
    for (auto & elem: as_array){
        using elem_t = element_t<T>;
        auto type_mismatch = check_type_mismatch<elem_t>(elem,path);
        if (type_mismatch){
            errors.push_back(*type_mismatch);
            return;
        }
        if (idx == 0){
            first_type = elem.type(); 
        } else if (elem.type() != first_type){
            errors.push_back(toml::make_error_info(
                "[Hetrogenous Array Error]",
                v.location(),
               std::format("Config contains hetrogenous array, arrays must be homogenous to be loaded.")
            ));
            return;
        }
        // TODO: ADD THE CURRENT ELEMENT TO PATH
        cfg::read_into(
            out.at(idx++),
            elem,
            errors,
            path
        );
    }
}

template<typename T>
auto read_into(
    T & out,
    toml::value const& v,
    std::vector<toml::error_info>& errors,
    std::string path
) -> void{
    using U = nocvref<T>;
    if constexpr( cfg::concepts::is_scalar_type<U>){
        // TODO: add the current identifier_of(^^v) to the path. Or do it before we actually go into a field.
        auto res = read_scalar<U>(v,path);
        if (!res){
            errors.push_back(res.error());
        }else{
            out = *res;
        }

    }else if constexpr (cfg::concepts::is_array_type<U>){
        read_array<U>(
            out,
            v,
            errors,
            path
        );

    }else if constexpr (cfg::concepts::is_struct_like<T>){
        if (!v.is_table()){
            errors.push_back( toml::make_error_info(
                "[Loading Error]",
                v.location(),
                std::format(
                    "Field '{}' expects table type, compatible with struct '{}',"
                    "but config contains a value of type '{}'. Field is unchanged/defaulted.",
                    path,
                    stable_type_name<T>(),
                    toml::to_string(v.type())
                )
            ));
            return;
        }
        auto table = v.as_table();
        template for(constexpr auto m: nonstatic_members_array<T>()){
            constexpr auto key = identifier_of(m);
            if (!v.contains(std::string(key))){
                errors.push_back( toml::make_error_info(
                    "[Missing Key]",
                    v.location(),
                    // TODO: add bold/underline or just general formatting to all of the errors
                    std::format( "\e[1;4m"
                        "Table '{}' expects field '{}', "
                        "but config does not contain a matching key. Field is unchanged/defaulted."
                        "\e[0m",
                        stable_type_name<T>(),
                        key
                    )
                ));
                continue;
            }
            cfg::read_into(
                out.[:m:],
                v.at(std::string(key)),
                errors,
                path + "." + std::string(key)
            );
        }
    }
}
} // NOTE: namespace cfg
#endif // CPP26
//#include "meta_helpers.hpp"
#include "toml11/fwd/error_info_fwd.hpp"
#include "primitive_types.hpp"
#include "cppslop.hpp"

#if !CPP26

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

inline auto print_errors(std::span< const toml::error_info> errors){
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
// TODO: Clarify behaviour on errors, make options to default or retain the previous value.
template<typename T>
auto parse_toml(T const& current, fs::path config_file_path) -> T{
    static constexpr bool dbg = true;
    LOG_DBG(dbg, "Parsing '{}'...",config_file_path);
    auto input = toml::try_parse(config_file_path, toml::spec::v(1,1,0));
    if (input.is_err()){
        auto errors = input.unwrap_err();
        LOG_ERROR("Failed to parse toml file '{}', {} errors :",config_file_path,errors.size());
        print_errors(errors);
        return current;
    }
    LOG_DBG(dbg, "Parsed '{}' succesfully. Reading values...", config_file_path);
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
