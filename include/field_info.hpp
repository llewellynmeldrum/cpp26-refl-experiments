#pragma once 
#include <format>
#include <optional>
#include <string_view>
#include <string>

#include "cfg_metadata.hpp"
#include "cppslop.hpp"
#include "zstring_view.hpp"
#include "auto_format.hpp"
template<std::formattable<char> T>
struct std::formatter<std::optional<T>>{
    constexpr auto parse(std::format_parse_context& ctx) noexcept{return ctx.begin();}
    template<typename U>
    constexpr auto format(std::optional<U> const& obj, auto& ctx) const noexcept{
        if (obj){
            return std::format_to(ctx.out(),"{}",*obj);
        }else{
            return std::format_to(ctx.out(),"nullopt");
        }
    }
};
struct AUTO_FORMAT FieldInfo{
    // a field should probably always have a name:
    // - if its has an identifier, its guaranteed, unless its anonynmous. I havent tested that, but i dont really plan on supporting it anyways.
    // - If it has a cfg::Name annotation
    // - if it doesnt have an identifier: 
    //      -> its an array element (in which case its name should be array_name[idx])
    //      -> its anonymous (this is unsupported, just error)
    //  Anyways, if it really truly doesnt have a name, just set its name to an empty string. Who cares.
    std::string type_name{"<no_type_name>"};
    std::optional<zstring_view> identifier_name{std::nullopt};
    std::string name{""};


    std::optional<cfg::Range> range;
    std::optional<zstring_view> note;
    std::optional<size_t> index{};
    bool is_root{false};
    constexpr auto is_array_element() const noexcept -> bool{
        return index.has_value();
    }
    constexpr auto ui_label() const  -> zstring_view{
        return name;
    }
    constexpr auto min_max_str() const -> std::string{
        ASSERT(has_range());
        return std::format(" min:{}, max:{}",range.value().min,range.value().max);
    }
    // Get the actual tooltip from cfg::Note annotation (if any)
    constexpr auto tooltip() const  -> std::optional<zstring_view>{
        return note;
    }


    // return the input tooltip, useful for sliders
    constexpr auto input_tooltip() const -> std::optional<std::string>{
        return has_range() ? make_optional(min_max_str()) : std::optional<std::string>{};
    }
    // Return the meta tooltip, i.e type name and true identifier (if applicable)
    constexpr auto meta_tooltip() const -> std::string {
        return std::format(
            "{} {}", 
            type_name,
            identifier_name ? *identifier_name : "(anonymous)"
        );
    }
    constexpr auto has_range() const noexcept->bool    {return range.has_value();}
    constexpr auto has_note () const noexcept->bool     {return note.has_value();}

    inline auto element_at(zstring_view type_name, size_t idx) -> FieldInfo{
        return {
            .type_name = type_name.clone(),
            .name = std::format("{}[{}]",name,idx),
            .range = range, // elements of an array inherit the range of their container
            .note = std::nullopt,
            .index = idx,
        };
    }
};

#include "common.hpp"
#if CPP26
// --- formerly field_info.refl.hpp ---
#include <meta>
template<std::meta::info m>
constexpr auto make_field_info(zstring_view type_name, bool is_root=false) -> FieldInfo{
    using namespace std::meta;
    constexpr auto note_anno = cfg::try_get_note(m);
    constexpr auto has_rename = try_get_annotation_of<cfg::Name>(m);
    auto identifier = has_identifier(m) ? zstring_view{std::define_static_string(identifier_of(m))} : std::optional<zstring_view>{};
    auto name = std::string{};
    if (has_rename){
        name = std::string(has_rename.value().txt);
    }else if (identifier){
        name = std::string(identifier.value().clone());
    } else {
        name = std::string{};
    }
    
    return FieldInfo{
        .type_name = type_name.clone(),
        .identifier_name = identifier,
        .name = std::move(name),
        .range = cfg::try_get_range(m),
        .note = note_anno ? zstring_view(note_anno.value().txt) : std::optional<zstring_view>{},
        .is_root = is_root,
    };
}
#endif // CPP26
static_assert(std::optional<zstring_view>{} == std::nullopt);
