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
    auto type = remove_cvref(type_of(m));
    return has_template_arguments(type) && template_of(type) == ^^Range;
}

template<typename T, std::meta::info m >
consteval inline auto metadata_from_refl() -> FieldMetadata<T>{
    using namespace std::meta;
    ASSERT(is_default_constructible_type(type_of(m)));
    auto m_type = remove_cvref(type_of(m));
    auto res = FieldMetadata<T>{};
    template for (constexpr auto anno: annotations_of(m)){
        if constexpr(is_range_annotation(m)){
            auto range_t = template_arguments_of(remove_cvref(type_of(anno)))[0];
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
consteval inline auto validate(T const& obj, std::string path) -> void{
    using namespace std::meta;

//    static_assert(false, display_string_of(remove_cvref(type_of(^^obj))) );
    template for (constexpr auto m: nonstatic_members_array<T>()){
        auto v = obj.[:m:];
        auto cur_path = path + std::string(".") + identifier_of(m);
        static_assert(has_default_member_initializer(m), 
            "\nERROR: Field '" + std::string(display_string_of(dealias(remove_cvref(^^T)))) + "::"+ std::string(identifier_of(m)) + "' lacks a default intializer. \n"
            "-> All members, sub-members (and so on) of config structs require default initializers."
        );
            
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
                static_assert((range.min <= v && v <= range.max), 
                    "Error: default value for '" + cur_path + "' is outside of its Range annotation.");
            }
        }
        if constexpr(cfg::meta::is_struct_like(remove_cvref(type_of(m)))){
            validate<decltype(v)>(v, cur_path);
        }
    }
}

// TODO: call this within a consteval block inside the root cfg concept or registration handler
template<typename C>
consteval inline auto validate_root_cfg() -> void{
    validate<C>(C{}, std::string(std::meta::identifier_of(^^C)));
}

} // NOTE: namespace cfg

