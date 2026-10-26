#pragma once 
#include "common.hpp"
#include "cfg_annotations.hpp"

#if CPP26
#include <meta>
#include <concepts>

#include "concept_helpers.hpp"
#include "meta_helpers.hpp"
#include "toml_traits.hpp"
#include "static_utils.hpp"
#include "cfg_registration.hpp" 
#include "annotation_helpers.hpp"
#include "static_styling.hpp"


namespace cfg{
template<typename T>
constexpr inline bool is_root_config_v = has_annotation<cfg::Registration>(^^T);

template<typename T>
concept is_root_config = is_root_config_v<T>;



consteval auto try_get_range(std::meta::info m) -> std::optional<cfg::Range>{
    return try_get_annotation_of<cfg::Range>(m);
}
consteval auto try_get_note(std::meta::info m) -> std::optional<cfg::Note>{
    return try_get_annotation_of<cfg::Note>(m);
}
consteval auto should_skip_field(std::meta::info m) -> bool{ 
    return has_annotation<cfg::Skip>(m);
}
template<typename T>
    requires is_root_config<T>
consteval auto get_registration() -> cfg::Registration{
    return *try_get_annotation_of<cfg::Registration>(^^T);
}
//consteval auto is_root_config(std::meta::info m) -> bool{
//    return has_annotation<cfg::Registration>(m);
//}



template<typename T, std::meta::info m> 
consteval auto type_accepts_annotation()->bool{
    using namespace std::meta;
    constexpr auto anno_t = remove_cvref(type_of(m));
    if constexpr(anno_t == ^^cfg::Note){
        return true;
    }else if constexpr(anno_t == ^^cfg::Range){
        if constexpr (is_numeric<T>){
            constexpr auto r = std::meta::extract<cfg::Range>(m);
            constexpr auto t = dealias(remove_cvref(^^T));

            static_assert(satisfies_concept(^^is_numeric, t), 
                "Range annotation is only valid for numeric types, '" + std::string(stable_type_name_meta(^^T)) + "' is NOT numeric."); // for the time being. This will need to be changed if i implement it for vector types or for arrays
            return true;
        }else{
            static_assert(false, "cfg::Range is only implemented for numeric types atm. It should probably be inherited by arrays and scalar members of a struct"
                "\nAnyways, annotation of cfg::Range is not applicable for type T='" 
                + std::string(display_string_of(dealias(^^T)))
                +"'. "
            );
            return false;
        }
    }else if constexpr(anno_t == ^^cfg::Skip){
        static_assert(false, "cfg::Skip is not implemented yet");
        return false;
    }else{
        static_assert(false, 
            "Unknown annotation '" + std::string(display_string_of(anno_t)) 
            + "' on type '" + std::string(display_string_of(dealias(^^T))) + "'."
        );
        return false;
    }
}
template<typename T, std::meta::info m> 
consteval auto is_declaration_valid(T const& obj) -> bool{
    using namespace ansi;
    using namespace std::meta;
    auto v = obj.[:m:];
    using Mt = [:remove_cvref(type_of(m)):];
    if constexpr (cfg::meta::is_scalar_type(type_of(m)) && !has_default_member_initializer(m)){
        static_error("Scalar field '"+
                     std::string(display_string_of(dealias(remove_cvref(^^T)))) + "::"+ std::string(identifier_of(m)) 
                     + "' lacks a default intializer.",
            "All scalar fields within a config struct and its descendent members require default initializers."
        );
        return false;
    }
    if constexpr (std::meta::is_const(type_of(m)) || std::meta::is_reference_type(type_of(m))){
        static_assert(false,
            "\nERROR: Field'" + std::string(display_string_of(dealias(remove_cvref(^^T)))) + "::"+ std::string(identifier_of(m)) 
            + "' has an invalid type: -> " + std::string(display_string_of(type_of(m))) 
            + " <- \n"
            "Config fields may not be const or reference qualified, as they must trivially modifiable.\n"
        );
        return false;
    }

    template for (constexpr auto anno: annotations_array<m>()){
        if (!type_accepts_annotation<Mt, anno>()){
            return false;
        }
    }
    return true;
}
template<typename T>
consteval inline auto static_validate_rec(T const& obj, std::string path) -> bool{
    using namespace ansi;
    using namespace std::meta;

//    static_assert(false, display_string_of(remove_cvref(type_of(^^obj))) );
    template for (constexpr auto m: nonstatic_members_array<T>()){
        using Mt = [:remove_cvref(type_of(m)):];
        auto cur_path = path + std::string(".") + identifier_of(m);
        
        auto v = obj.[:m:];
        constexpr auto range_anno = try_get_range(m);
        if constexpr(range_anno){
            constexpr auto range_raw = *try_get_raw_annotation_of<cfg::Range>(m);
            if constexpr(type_accepts_annotation<Mt, range_raw>()){
                auto r = *range_anno;
                if (!(r.min <= v && v <= r.max)){
                    throw std::logic_error(
                        "ERROR: default value for '" + std::string(identifier_of(m))+ "' ("+ ansi::tostr(static_cast<i32>(v)) +") is outside of its Range annotation"
                        "-> ["+ ansi::tostr(static_cast<i32>(r.min)) +", "+ ansi::tostr(static_cast<i32>(r.max))+"]."
                    );
                    return false;
                }
            }
        }
        is_declaration_valid<T,m>(obj);
        if constexpr(cfg::meta::is_struct_like(remove_cvref(type_of(m)))){
            static_validate_rec<decltype(v)>(v, cur_path);
        }
        if constexpr (cfg::meta::is_array_type(remove_cvref(type_of(m)))){
            static_validate_rec<typename Mt::value_type>(v[0], cur_path + "[0]");
        }
    }
    return true;
}


// Recursively checks annotations for obvious errors
// Checks:
// - default values are within the annotated range
// - if there is a range annotation, there is only one
// - if there is a range annotation, its template type matches the type of the variable
// - if there is a range annotation, the type is numeric
template<typename C>
consteval inline auto static_validate_root_cfg() -> bool{
    return static_validate_rec<C>(C{}, std::string(std::meta::identifier_of(^^C)));
}
template<typename T>
concept is_valid_root_config = static_validate_root_cfg<T>();

template<typename T>
inline auto validate_recursive(T const& obj, std::string path) -> bool{
    using namespace std::meta;

    template for (constexpr auto m: nonstatic_members_array<T>()){
        using Ft = [:dealias(remove_cvref(type_of(m))):];
        auto val = obj.[:m:];
        auto cur_path = path + std::string(".") + identifier_of(m);
        constexpr auto range_anno = try_get_range(m);
        if constexpr(range_anno){
            constexpr auto range_raw = *try_get_raw_annotation_of<cfg::Range>(m);
            if constexpr(type_accepts_annotation<Ft, range_raw>()){
                auto r = *range_anno;
                bool lt_range =  val < r.min;
                bool gt_range =  val > r.max;
                if (lt_range || gt_range){
                    LOG_WARN(
                        "Field '{}' contains value ({}) outside of range [{},{}] ({})",
                        cur_path, val, r.min, r.max,
                        lt_range 
                        ? std::format("{} < {}",val,r.min)
                        : std::format("{} > {}",val,r.max)
                    );
                    return false;
                }
            }
        }
        if constexpr(cfg::concepts::is_struct_like<Ft>){
            validate_recursive<Ft>(val, cur_path);
        }
        if constexpr (cfg::concepts::is_array_type<Ft>){
            validate_recursive<typename Ft::value_type>(val[0], cur_path + "[0]");
        }
    }
    return true;
}

template<typename C>
inline auto validate_root_cfg(C const& v) -> bool{
    return validate_recursive<C>(v, std::string(stable_type_name<C>()));
}

template<typename T, std::meta::info field>
[[nodiscard]] inline auto validate_range(T const& v) -> bool{
    constexpr auto raw_type_name = stable_type_name_meta(meta::nocvref_alias(type_of(field)));
    using Ft = typename[:dealias(remove_cvref(type_of(field))):];
    constexpr auto has_range = try_get_range(field);
    if constexpr (has_range){
        auto r = *has_range;
        return r.min <= v && v <= r.max;
    }else {
        return true;
    }

}

} // NOTE: namespace cfg
#endif // CPP26
