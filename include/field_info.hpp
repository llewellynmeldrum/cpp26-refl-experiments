#pragma once 
#include <format>
#include <optional>
#include <string_view>

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
    std::optional<std::string_view> name;

    std::optional<cfg::Range> range;
    std::optional<std::string_view> note;
    std::optional<size_t> index{};
    bool is_root{false};
    constexpr auto note_zstring()->std::optional<zstring_view>{
        if (note){
            return zstring_view(note->data(), note->size());
        }else{
            return std::nullopt;
        }
    }
    constexpr auto has_name()->bool    {return name.has_value();}
    constexpr auto has_range()->bool    {return range.has_value();}
    constexpr auto has_note()->bool     {return note.has_value();}
    constexpr auto tooltip()->std::optional<std::string_view> {return note;}
    constexpr auto element_at(size_t idx) -> FieldInfo{
        return {
            .name = name,
            .range = range,
            .note = std::nullopt,
            .index = idx,
        };
    }
    constexpr auto get_label() -> std::string{
        if (index){
            return std::format("##{}[{}]", name.value_or(""sv),*index );
        }else{
            return std::format("{}", name.value_or(""sv) );
        }
    }
};

#include "common.hpp"
#if CPP26
// --- formerly field_info.refl.hpp ---
#include <meta>
template<std::meta::info m>
consteval auto make_field_info(bool is_root=false) -> FieldInfo{
    constexpr auto note_anno = cfg::try_get_note(m);
    return FieldInfo{
        .name = std::meta::has_identifier(m) ? identifier_of(m) : std::optional<std::string_view>{},
        .range = cfg::try_get_range(m),
        .note = note_anno ? std::string_view(note_anno.value().txt) : std::optional<std::string_view>{},
        .is_root = is_root,
    };
}
#endif // CPP26
