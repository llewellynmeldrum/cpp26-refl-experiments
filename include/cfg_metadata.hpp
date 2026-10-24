#pragma once 
#include <format>
#include <optional>

#include <libassert/assert.hpp>

#include "primitive_types.hpp"
#include "static_styling.hpp"



#if CLANGD_PARSE
    #define CFG_RANGE(min,max) 
    #define CFG_NOTE(txt) 
    #define CFG_SKIP  
#else
    #define CFG_RANGE(min,max)  [[=cfg::Range{min,max}]]
    #define CFG_NOTE(txt)       [[=cfg::Note{std::define_static_string(txt)}]]
    #define CFG_SKIP            [[=cfg::Skip{}]]
#endif
namespace cfg {
// NOTE: Maybe Range for string should represent the min and max length of the string
// TODO: as a rule, scalar elements of containers should inherit the range of their container
// Also i think that scalar members of a struct with a range should get them too 
struct Range{
    f64 min{-std::numeric_limits<f64>::infinity()};
    f64 max{+std::numeric_limits<f64>::infinity()}; 
    template<typename T>
    static constexpr auto default_value()->Range{
        return {
            .min = static_cast<f64>(std::numeric_limits<T>::lowest() / 2.0),
            .max = static_cast<f64>(std::numeric_limits<T>::max() / 2.0),
        };
    }
};
struct Note{ char const* txt{}; };
struct Skip{ [[maybe_unused]] char const* reason{};};

} // NOTE: namespace cfg
namespace cfg{
template<typename T>
struct FieldMetadata{
    using value_type = T;
    T                               m_default_val;
    std::optional<cfg::Range>    m_range              {std::nullopt};
    const char*                     m_key_identifier     {nullptr};
    const char*                     m_note               {nullptr};
    bool                            m_should_skip{};
    constexpr auto key() const -> const char* {
        return m_key_identifier;
    }
    constexpr auto range() const -> cfg::Range {
        ASSERT(m_range.has_value());
        return *m_range;
    }
    constexpr auto is_inside_range(T const& val) const -> bool {
        ASSERT(m_range.has_value());
        return (range().min <= val && val <= range().max);
    }
};
} // NOTE: namespace cfg

#include "common.hpp"
#if CPP26
// --- formerly cfg_metadata.refl.hpp ---
#include <meta>
#include <concepts>

#include "concept_helpers.hpp"
#include "meta_helpers.hpp"
#include "toml_traits.hpp"
#include "static_utils.hpp"
#include "cfg_registration.hpp" // cfg::Registration, used by validate_recursive
#include "annotation_helpers.hpp"

namespace cfg{


consteval auto try_get_range(std::meta::info m) -> std::optional<cfg::Range>{
    return try_get_annotation_of<cfg::Range>(m);
}
consteval auto try_get_note(std::meta::info m) -> std::optional<cfg::Note>{
    return try_get_annotation_of<cfg::Note>(m);
}
consteval auto should_skip_field(std::meta::info m) -> bool{ 
    return has_annotation<cfg::Skip>(m);
}



template<typename T, std::meta::info m> 
consteval auto type_accepts_annotation()->bool{
    using namespace std::meta;
    constexpr auto anno_t = remove_cvref(type_of(m));
    if constexpr(anno_t == ^^cfg::Note){
        return true;
    }else if constexpr(anno_t == ^^cfg::Range){
        if constexpr (is_numeric<T>){
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
template<typename T>
consteval inline auto static_validate_rec(T const& obj, std::string path) -> bool{
    using namespace ansi;
    using namespace std::meta;

//    static_assert(false, display_string_of(remove_cvref(type_of(^^obj))) );
    template for (constexpr auto m: nonstatic_members_array<T>()){
        using Mt = [:remove_cvref(type_of(m)):];
        auto v = obj.[:m:];
        auto cur_path = path + std::string(".") + identifier_of(m);
        
        if constexpr (cfg::meta::is_scalar_type(type_of(m)) && !has_default_member_initializer(m)){
            static_error("Scalar field '"+
                         std::string(display_string_of(dealias(remove_cvref(^^T)))) + "::"+ std::string(identifier_of(m)) 
                         + "' lacks a default intializer.",
                "All scalar fields within a config struct and its descendent members require default initializers."
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
            static_assert(type_accepts_annotation<Mt, anno>());
        }
            
        constexpr auto range_anno = try_get_range(m);
        if constexpr(range_anno){
            constexpr auto r = *range_anno;
            constexpr auto t = dealias(remove_cvref(type_of(m)));

            static_assert(satisfies_concept(^^is_numeric, t), 
                "Range annotation is only valid for numeric types"); // for the time being. This will need to be changed if i implement it for vector types or for arrays
            if (!(r.min <= v && v <= r.max)){
                throw std::logic_error(
                    "ERROR: default value for '" + cur_path + "' ("+ tostr(static_cast<i32>(v)) +") is outside of its Range annotation"
                    "-> ["+ tostr(static_cast<i32>(r.min)) +", "+ tostr(static_cast<i32>(r.max))+"]."
                );
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
constexpr inline bool valid_config_struct_v = static_validate_root_cfg<T>();

template<typename T>
concept valid_config_struct = valid_config_struct_v<T>;

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
