#pragma once 
#include <meta>
#include <concepts>

#include "cfg_metadata.hpp"
#include "concept_helpers.hpp"
#include "meta_helpers.refl.hpp"
#include "toml_traits.refl.hpp"
#include "static_utils.refl.hpp"

namespace cfg{
template<typename AnnoType>
consteval auto try_get_annotation_of(std::meta::info m) -> std::optional<AnnoType>{
    auto v = annotations_of_with_type(m, ^^AnnoType);
    if (v.empty()){
        return std::nullopt;
    }else{
        return std::meta::extract<AnnoType>(v[0]);
    }
}
template<typename AnnoType>
consteval auto has_annotation(std::meta::info m) -> bool{
    auto v = annotations_of_with_type(m, ^^AnnoType);
    return v.empty();
}

consteval auto is_range_annotation(std::meta::info m) -> bool{
    return is_annotation_of<cfg::Range>(m);
}
consteval auto is_note_annotation(std::meta::info m) -> bool{
    return is_annotation_of<cfg::Note>(m);
}
consteval auto is_skip_annotation(std::meta::info m) -> bool{
    return is_annotation_of<cfg::Skip>(m);
}
consteval auto try_get_range(std::meta::info m) -> std::optional<cfg::Range>{
    return try_get_annotation_of<cfg::Range>(m);
}
consteval auto try_get_note(std::meta::info m) -> std::optional<cfg::Note>{
    return try_get_annotation_of<cfg::Note>(m);
}
consteval auto should_skip_field(std::meta::info m) -> bool{ 
    return has_annotation<cfg::Skip>(m);
}

consteval auto has_rgba_annotation(std::meta::info m) -> bool{
    return has_annotation<cfg::RGBA>(m);
}
consteval auto has_rgb_annotation(std::meta::info m) -> bool{
    return has_annotation<cfg::RGB>(m);
}
consteval auto has_xyz_annotation(std::meta::info m) -> bool{
    return has_annotation<cfg::XYZ>(m);
}
consteval auto has_xyzw_annotation(std::meta::info m) -> bool{
    return has_annotation<cfg::XYZW>(m);
}


template<typename T>
consteval inline auto static_validate_rec(T const& obj, std::string path) -> bool{
    using namespace std::meta;

//    static_assert(false, display_string_of(remove_cvref(type_of(^^obj))) );
    template for (constexpr auto m: nonstatic_members_array<T>()){
        auto v = obj.[:m:];
        auto cur_path = path + std::string(".") + identifier_of(m);
        if constexpr (cfg::meta::is_scalar_type(type_of(m))){
            static_assert(has_default_member_initializer(m), 
                "\nERROR: Scalar field '" + std::string(display_string_of(dealias(remove_cvref(^^T)))) + "::"+ std::string(identifier_of(m)) + "' lacks a default intializer. \n"
                "-> All scalar fields within a config struct and its descendent members require default initializers."
            );
        }
        if constexpr (std::meta::is_const(type_of(m)) || std::meta::is_reference_type(type_of(m))){
            static_assert(false,
                "\nERROR: Field'" + std::string(display_string_of(dealias(remove_cvref(^^T)))) + "::"+ std::string(identifier_of(m)) 
                + "' has an invalid type: -> " + std::string(display_string_of(type_of(m))) 
                + " <- \n"
                "Config fields may not be const or reference qualified, as they must trivially modifiable.\n"
            );

        }
            
        constexpr auto range_anno = try_get_range(m);
        if constexpr(range_anno){
            constexpr auto r = *range_anno;
            constexpr auto t = dealias(remove_cvref(type_of(m)));

            static_assert(satisfies_concept(^^is_numeric, t), 
                "Range annotation is only valid for numeric types"); // for the time being. This will need to be changed if i implement it for vector types or for arrays
            if (!(r.min <= v && v <= r.max)){
                throw std::logic_error(
                    "ERROR: default value for '" + cur_path + "' is outside of its Range annotation.");
            }
        }

        if constexpr(cfg::meta::is_struct_like(remove_cvref(type_of(m)))){
            static_validate_rec<decltype(v)>(v, cur_path);
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
inline auto validate_recursive(T const& obj, std::string path) -> bool{
    using Reg = cfg::Registration<T>;
    using namespace std::meta;

    template for (constexpr auto m: nonstatic_members_array<T>()){
        using Ft = [:dealias(remove_cvref(type_of(m))):];
        auto val = obj.[:m:];
        auto cur_path = path + std::string(".") + identifier_of(m);
        constexpr auto range_anno = try_get_range(m);
        if constexpr(range_anno){
            auto r = *range_anno;
            bool lt_range =  val < r.min;
            bool gt_range =  val > r.max;
            if (lt_range || gt_range){
                LOG_WARN(
                    "Field '{}' (of config '{}') contains value ({}) outside of range [{},{}] ({})",
                    Reg::config_name, cur_path, val, r.min, r.max,
                    lt_range 
                    ? std::format("{} < {}",val,r.min)
                    : std::format("{} > {}",val,r.max)
                );
                return false;
            }
        }
        if constexpr(cfg::concepts::is_struct_like<Ft>){
            validate_recursive<Ft>(val, cur_path);
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
    constexpr auto raw_type_name = stable_type_name(meta::nocvref_alias(type_of(field)));
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

