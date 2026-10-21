#pragma once 
#include "cfg_metadata.hpp"
#include "cppslop.hpp"
#include <string_view>
struct FieldInfo{
    std::optional<std::string_view> name;
    std::optional<cfg::Range> range;
    std::optional<std::string_view> note;
    std::optional<size_t> index{};
    constexpr auto has_name()->bool    {return name.has_value();}
    constexpr auto has_range()->bool    {return range.has_value();}
    constexpr auto has_note()->bool     {return note.has_value();}
    constexpr auto tooltip()->std::optional<std::string_view> {return note;}
    constexpr auto element_of(size_t idx) -> FieldInfo{
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
