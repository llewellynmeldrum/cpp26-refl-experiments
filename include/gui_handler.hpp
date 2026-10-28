#pragma once 

#include <meta>
#include <numeric>
#include <optional>

#include <libassert/assert.hpp>
#include "libassert_wrapper.hpp"
#include "common.hpp"
#include "imgui_wrapper.hpp"
#include "cfg_traits.hpp"
#include "field_info.hpp"
#include "stable_type_name.hpp"
#include "cppslop.hpp"
#include "cfg_helpers.hpp"
#include "cfg_metadata.hpp"
#include "cfg_toml_rw.hpp"
#include "toml_traits.hpp"
#include "meta_helpers.hpp"
#include "auto_format.hpp"

#include "lmgui_tables.hpp"
#include "lmgui_windows.hpp"

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
    ASSERT_IF_NOT_CLANG(!info.has_range(), "Strings do not support ranges atm. Consider implementing to constrain string length min and max?",v,std::format("{}",info));
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
        auto spec = ui::TableSpec{}
            .set_name(array_name)
            .set_flags(ui::TableFlagBits::eBorders | ui::TableFlagBits::eRowBg)
            .set_column_headers({
                {"idx", ui::TableColumnFlagBits::eWidthFixed},
                {"value", ui::TableColumnFlagBits::eWidthStretch},
            });

        if (auto table = ui::begin_scope(spec)){
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
        auto spec = ui::TableSpec{}
            .set_name(array_name)
            .set_flags(ui::TableFlagBits::eBorders | ui::TableFlagBits::eRowBg)
            .add_column_header("idx", ui::TableColumnFlagBits::eWidthFixed);

        template for (constexpr auto m : nonstatic_members_array<elem_t>()){
            auto member_name = std::string(identifier_of(m));
            auto info = make_field_info<m>();
            auto hint = std::string{};
            if (info.note){
                hint = info.note.value();
            }
            spec.add_column_header(member_name, ui::TableColumnFlagBits::eWidthStretch,std::string(hint));
            // add a hint here with the info?
        }

        if (auto table = ui::begin_scope(spec)){
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
    if (struct_info.is_root){
        open = ImGui::BeginTable(std::string(name).c_str(),2,ImGuiTableFlags_Borders);
        if (!open){ return false; } // exit early, dont iterate anything
    }
    auto spec = ui::PropertyGridSpec{}
        .set_id(name);
    if (auto grid = ui::begin_scope(spec)){
        template for (constexpr auto m: nonstatic_members_array<T>()){
            using Mt = [:dealias(remove_cvref(type_of(m))):];
            auto info = make_field_info<m>();
            if constexpr (concepts::is_scalar_type<Mt>){
                grid.scalar_row(info.name_zstring(), info.note_zstring(), [&]{ edited |= edit_leaf(val.[:m:], info);});
            }else if constexpr (concepts::is_array_type<Mt>){
//                edited |= edit_array_like(val.[:m:], info);
            }else if constexpr (concepts::is_struct_like<Mt>){
                grid.struct_row(info.name_zstring(), info.note_zstring(), [&]{ edited |= edit_struct_like(val.[:m:], info);});
 //               edited |= edit_struct_like(val.[:m:], info);
            }
        }
    }
    if (struct_info.is_root){
        ImGui::EndTable();
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
    auto res = ConfigActions{};

    auto const main_window_pos = glm::vec2{
        ui::ViewportSize().x - main_window_size.x,
        0.0f,
    };
    auto window_spec = ui::WindowSpec{}
        .set_size(main_window_size)
        .set_pos(main_window_pos)
        .set_name(reg.config_name);

    if (auto window = ui::begin_scope(window_spec)){
        auto & root = storage.live;
        res.edited = false;
        ig::ShowMetricsWindow();
        auto child_spec = ui::ChildWindowSpec{}
            .set_name("Srolling Body")
            .set_size({0,-footer_height})
        ;
        if (auto child = ui::begin_scope(child_spec)){
            edit_struct_like(root,make_field_info<^^T>(true));
        }
        if (auto child = ui::begin_scope(child_spec)){
            ig::Separator();
            auto footer = ui::raii::ChildWindow("##",{0,footer_height});
            res.save_to_disk = ui::Button( "Save to file",
                std::format("Saves the current live config state to '{}'.",storage.filename)
            );
            ui::SameLine();

            res.load_from_disk = ui::Button( "Load from file", 
                std::format("Loads the config state from '{}'.",storage.filename)
            );
            ui::SameLine();

            res.reset_to_default = ui::Button( "Reset to defaults",
                "Resets all fields to defaults, according to their initializers."
            );
            ui::SameLine();
        }
    }
    return res;
}

} // NOTE: namespace cfg
#endif // CPP26
