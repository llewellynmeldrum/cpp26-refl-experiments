#pragma once 
#include "cfg_metadata.hpp"
#include <meta>
#include <concepts>
#include "concept_helpers.hpp"
#include "meta_helpers.refl.hpp"
#include "toml_traits.refl.hpp"
#include "static_utils.refl.hpp"

namespace cfg{
inline consteval auto is_range_annotation(std::meta::info m) -> bool{
    using namespace std::meta;
    auto type = dealias(remove_cvref(type_of(m)));
    return has_template_arguments(type) && template_of(type) == ^^Range;
}
inline consteval auto is_note_annotation(std::meta::info m) -> bool{
    using namespace std::meta;
    auto type = dealias(remove_cvref(type_of(m)));
    return type == ^^cfg::Note;
}
inline consteval auto is_skip_annotation(std::meta::info m) -> bool{
    using namespace std::meta;
    auto type = dealias(std::meta::remove_cvref(type_of(m)));
    return type == ^^cfg::Skip;
}

template<typename T, std::meta::info m>
inline consteval auto try_get_range() -> std::optional<cfg::Range<T>>{
    auto v = annotations_of_with_type(m, ^^cfg::Range<T>);
    if (v.empty()) return std::nullopt;
    else return std::meta::extract<cfg::Range<T>>(v[0]);
    return std::nullopt;
}
template<std::meta::info m>
inline consteval auto try_get_note() -> std::optional<cfg::Note>{
    auto v = annotations_of_with_type(m, ^^cfg::Note);
    if (v.empty()) return std::nullopt;
    else return std::meta::extract<cfg::Note>(v[0]);
}
template<std::meta::info m>
inline consteval auto should_skip_field() -> bool{ 
    auto v = annotations_of_with_type(m, ^^cfg::Skip);
    if (v.empty()) return false;
    else return true;
}



template<typename T, std::meta::info m >
consteval inline auto metadata_from_refl() -> FieldMetadata<T>{
    using namespace std::meta;
    auto res = FieldMetadata<T>{};
    ASSERT(is_default_constructible_type(type_of(m)));
    constexpr auto m_type = dealias(remove_cvref(type_of(m)));
    constexpr auto parent_type = dealias(remove_cvref((type_of(parent_of(m)))));
    using pT = [:parent_type:];
    constexpr T default_val = pT{}.[:m:];
    res.m_default_val = default_val;
    template for (constexpr auto anno: annotations_of(m)){
        if constexpr(is_range_annotation(m)){
            constexpr auto range_t = template_arguments_of(remove_cvref(type_of(anno)))[0];
            static_assert(range_t == dealias(remove_cvref(type_of(m))), "Type of range must match object type.");
            res.range = [:anno:];
        }else if constexpr (type_of(anno) == ^^cfg::Note){
            res.note = std::define_static_string([:anno:].txt);
        }else{
            static_assert(false, "Unknown annotation: cannot be parsed.");
        }
    }
    return res;
}

template<typename T>
consteval inline auto validate(T const& obj, std::string path) -> bool{
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
            
        template for (constexpr auto anno: annotations_array<m>()){
            if constexpr(is_range_annotation(anno)){
                using R = [:remove_cvref(type_of(anno)):];
                constexpr auto t = dealias(remove_cvref(type_of(m)));
                constexpr auto range_t = dealias(template_arguments_of(remove_cvref(type_of(anno)))[0]);
                constexpr auto range = extract<R>(anno);

                static_assert(satisfies_concept(^^is_numeric, t), 
                    "Range annotation is only valid for numeric types");
                static_assert(range_t == t, 
                    "Type of range must match object type.");
                if (!(range.min <= v && v <= range.max)){
                    throw std::logic_error(
                        "ERROR: default value for '" + cur_path + "' is outside of its Range annotation.");
                }
            }
        }

        if constexpr(cfg::meta::is_struct_like(remove_cvref(type_of(m)))){
            validate<decltype(v)>(v, cur_path);
        }
    }
    return true;
}

// TODO: call this within a consteval block inside the root cfg concept or registration handler
template<typename C>
consteval inline auto validate_root_cfg() -> bool{
    return validate<C>(C{}, std::string(std::meta::identifier_of(^^C)));
}

} // NOTE: namespace cfg

