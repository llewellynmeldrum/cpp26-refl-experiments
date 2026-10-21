#pragma once 
#include <meta>
#include "field_info.hpp"
#include "cfg_metadata.refl.hpp"
template<std::meta::info m>
consteval auto make_field_info() -> FieldInfo{
    constexpr auto note_anno = cfg::try_get_note(m);
    return FieldInfo{
        .name = std::meta::has_identifier(m) ? identifier_of(m) : std::optional<std::string_view>{},
        .range = cfg::try_get_range(m),
        .note = note_anno ? std::string_view(note_anno.value().txt) : std::optional<std::string_view>{},
    };
}
