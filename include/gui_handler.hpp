#pragma once 

#include <meta>
#include <numeric>
#include <optional>

#include "common.hpp"
#include "imgui_wrapper.hpp"
#include "cfg_traits.hpp"
#include "field_info.hpp"
#include "lmgui_tables.hpp"
#include "stable_type_name.hpp"
#include "cppslop.hpp"
#include "cfg_helpers.hpp"
#include "cfg_metadata.hpp"
#include "toml_traits.hpp"
#include "meta_helpers.hpp"
#include "cfg_toml_rw.hpp"
#include "auto_format.hpp"

namespace cfg{

// NOTE: on Leaf
// leaves are nodes which can be represented by a single widget.
// Examples include primitive types (i32, f32, etc), and glm::vec types (glm::vec3, glm::vec4). glm::vec types are yet to be implemented.
template<typename T>
    requires concepts::is_scalar_integral<T> || concepts::is_scalar_floating_point<T>
auto edit_leaf(T& v, FieldInfo info) -> bool{
    auto label = info.get_label();
    auto range = info.range.value_or(Range::default_value<T>());
    bool changed = ui::ClampedSlider<T>(label, v, range.min,range.max, info.tooltip());
    return changed;
}

template<typename T>
    requires concepts::is_string_type<T>
auto edit_leaf(T& v, FieldInfo info) -> bool{
    auto label = info.get_label();
    ASSERT(!info.has_range(), "Strings do not support ranges atm. Consider implementing to constrain string length min and max?",v,std::format("{}",info));
    bool changed = ui::InputString(zstring_view(label), v, info.tooltip());
    return changed;
}
template<typename T>
    requires same_as_nocvref<T,bool>
auto edit_leaf(T& v, FieldInfo info) -> bool{
    auto label = info.get_label();
    auto range = info.range.value_or(Range::default_value<T>());
    bool changed = ui::ClampedSlider<T>(label, v, range.min,range.max, info.tooltip());
    return changed;
}
template<typename T>
    requires concepts::is_enum<T> 
auto edit_leaf(T& v, FieldInfo info) -> bool{
    auto label = info.get_label();
    auto hint = info.note_zstring();
    return ui::EnumDropdown<T>(label,v, hint);
}

#if CPP26

template<typename T> requires concepts::is_array_type<T>
auto edit_array_like(T& arr, FieldInfo info) -> bool{
    bool changed = false;
    using elem_t = element_t<T>;
    static_assert(concepts::is_scalar_type<elem_t> || concepts::struct_of_scalars<elem_t>,
        "Elements of arrays must be either scalar, or structs containing only scalars.");
    auto ui_id = std::string("##"+ std::string(stable_type_name<T>()));
    auto array_name = std::string{info.name.value_or("unnamed array??")};
    auto flags = ui::TableFlagBits::eBorders | ui::TableFlagBits::eRowBg;
    ui::Label(array_name, info.tooltip());
    if constexpr(concepts::is_scalar_type<elem_t>){
        // arrays of scalars -> simple table
        auto table = ui::Table{}
            .set_name(array_name)
            .set_flags(ui::TableFlagBits::eBorders | ui::TableFlagBits::eRowBg)
            .set_column_headers({
                {"index", ui::TableColumnFlagBits::eWidthFixed},
                {"value", ui::TableColumnFlagBits::eWidthStretch},
            });
        table.begin();

        if (table.is_shown()){
            auto idx = 0uz;
            for (auto& v: arr){
                table.next_row()
                    .column([&]{
                        ui::LabelF("[{}]",idx);
                    })
                    .column([&]{
                        changed |= edit_leaf(v, info.element_at(idx));
                    });
                idx++;
            }
        }
    }else{
        auto table = ui::Table{}
            .set_name(array_name)
            .set_flags(ui::TableFlagBits::eBorders | ui::TableFlagBits::eRowBg)
        ;
        table.add_column_header("index", ui::TableColumnFlagBits::eWidthFixed);
        template for (constexpr auto m : nonstatic_members_array<elem_t>()){
            auto member_name = std::string(identifier_of(m));
            auto info = make_field_info<m>();
            auto hint = std::string{};
            if (info.note){
                hint = info.note.value();
            }
            table.add_column_header(member_name, ui::TableColumnFlagBits::eWidthStretch,std::string(hint));
            // add a hint here with the info?
        }
        table.begin();

        if (table.is_shown()){
            auto idx = 0uz;
            for (auto& element: arr){
                // each element is a row
                auto idx_str = std::format("[{}]",idx);
                table.next_row().column([&]{ ui::Label(idx_str); });
                template for (constexpr auto m : nonstatic_members_array<elem_t>()){
                    auto info = make_field_info<m>();
                    if (info.name){
                        info.name.value() = "##" + idx_str + info.name.value(); 
                    }
                    info.note = std::nullopt; // note is placed in header column
                    table.column([&]{
                        changed |= edit_leaf(element.[:m:], info);
                    });
                }
                idx++;
            }
        }
    }
    return changed;
}
template<typename T> requires concepts::is_struct_like<T>
auto edit_struct_like(T& val, FieldInfo struct_info) -> bool{
    using namespace std::meta;
    auto name = struct_info.name.value_or("no struct name???"sv);
    bool edited = false;
    bool open = false;
    if (!struct_info.is_root){
        open = ui::BeginTreeNode(std::string(name));
        if (!open){ return false; } // exit early, dont iterate anything
    }
    template for (constexpr auto m: nonstatic_members_array<T>()){
        using Mt = [:dealias(remove_cvref(type_of(m))):];
        auto info = make_field_info<m>();
        if constexpr (concepts::is_scalar_type<Mt>){
            edited |= edit_leaf(val.[:m:], info);
        }else if constexpr (concepts::is_array_type<Mt>){
            edited |= edit_array_like(val.[:m:], info);
        }else if constexpr (concepts::is_struct_like<Mt>){
            edited |= edit_struct_like(val.[:m:], info);
        }
    }
    if (!struct_info.is_root){
        ui::EndTreeNode();
    }
    return edited;
}

struct ConfigActions{
    bool edited;
    bool save_to_disk;
    bool load_from_disk;
    bool reset_to_default;
};
template<typename T>
auto edit_config_window(Storage<T> & storage) -> ConfigActions{
    auto reg = cfg::get_registration<T>();
    auto const main_window_size = ui::ViewportSize() / 2.0f;
    auto const footer_height = ui::ElementHeight();
    auto const content_height = ui::AvaliableRegion().y - footer_height - ui::ItemSpacing().y;

    auto main_window = ui::raii::Window(reg.config_name, {
        .size= main_window_size,
    });
    auto & root = storage.live;
    bool edited = false;
    ig::ShowMetricsWindow();
    if (auto w = ui::raii::ChildWindow("SrollingBody",{0, content_height}); w){
        auto info = make_field_info<^^T>(true);
        edit_struct_like(root,info);
    }
    bool save_to_disk{}, load_from_disk{}, reset_to_default{};
    {
        ig::Separator();
        auto footer = ui::raii::ChildWindow("##",{0,footer_height});
        save_to_disk = ui::Button(
            "Save to file",
            std::format("Saves the current live config state to '{}'.",storage.filename)
        );
        ui::SameLine();

        load_from_disk = ui::Button(
            "Load from file", 
            std::format("Loads the config state from '{}'.",storage.filename)
        );
        ui::SameLine();

        reset_to_default = ui::Button(
            "Reset to defaults",
            "Resets all fields to defaults, according to their initializers."
        );
        ui::SameLine();
    }

    return {
        .edited           = edited,
        .save_to_disk     = save_to_disk,
        .load_from_disk   = load_from_disk,
        .reset_to_default = reset_to_default,
    };
}

} // NOTE: namespace cfg
#endif // CPP26
