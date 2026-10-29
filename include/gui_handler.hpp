#pragma once 

#include <imgui.h>
#include <meta>
#include <numeric>
#include <ranges>
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
#include "meta_helpers.hpp"

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

//template<typename T> requires concepts::is_array_type<T>
//auto edit_array_like(T& arr, FieldInfo info) -> bool{
//    bool changed = false;
//    using elem_t = element_t<T>;
//    static_assert(concepts::is_scalar_type<elem_t> || concepts::struct_of_scalars<elem_t>,
//        "Elements of arrays must be either scalar, or structs containing only scalars.");
//    auto ui_id = std::string("##"+ std::string(stable_type_name<T>()));
//    auto array_name = std::string{info.name.value_or("unnamed array??")};
//    auto flags = ui::TableFlagBits::eBorders | ui::TableFlagBits::eRowBg;
//    ui::Label(array_name, info.tooltip());
//    if constexpr(concepts::is_scalar_type<elem_t>){
//        // arrays of scalars -> simple table
//        auto spec = ui::TableSpec{}
//            .set_title(array_name)
//            .set_flags(ui::TableFlagBits::eBorders | ui::TableFlagBits::eRowBg)
//            .set_column_headers({
//                {"idx", ui::TableColumnFlagBits::eWidthFixed},
//                {"value", ui::TableColumnFlagBits::eWidthStretch},
//            });
//
//        if (auto table = ui::begin_scope(spec)){
//            auto idx = 0uz;
//            for (auto& v: arr){
//                table.next_row()
//                    .column([&]{
//                        ui::LabelF("[{}]",idx);
//                    })
//                    .column([&]{
//                        changed |= edit_leaf(v, info.element_at(idx));
//                    });
//                idx++;
//            }
//        }
//    }else{
//        auto spec = ui::TableSpec{}
//            .set_title(array_name)
//            .set_flags(ui::TableFlagBits::eBorders | ui::TableFlagBits::eRowBg)
//            .add_column_header("idx", ui::TableColumnFlagBits::eWidthFixed);
//
//        template for (constexpr auto m : nonstatic_members_array<elem_t>()){
//            auto member_name = std::string(identifier_of(m));
//            auto info = make_field_info<m>();
//            auto hint = std::string{};
//            if (info.note){
//                hint = info.note.value();
//            }
//            spec.add_column_header(member_name, ui::TableColumnFlagBits::eWidthStretch,std::string(hint));
//            // add a hint here with the info?
//        }
//
//        if (auto table = ui::begin_scope(spec)){
//            auto idx = 0uz;
//            for (auto& element: arr){
//                // each element is a row
//                table.next_row().column([&]{ ui::Label(idx_str); });
//                template for (constexpr auto m : nonstatic_members_array<elem_t>()){
//                    auto info = make_field_info<m>();
//                    if (info.name){
//                        info.name.value() = "##" + idx_str + info.name.value(); 
//                    }
//                    info.note = std::nullopt; // note is placed in header column
//                    table.column([&]{
//                        changed |= edit_leaf(element.[:m:], info);
//                    });
//                }
//                idx++;
//            }
//        }
//    }
//    return changed;
//}
template<typename T> requires concepts::is_struct_like<T>
[[nodiscard]] auto edit_members_of(ui::PropertyGridScope& grid, T& val, FieldInfo struct_info) -> bool;

template<typename T> 
[[nodiscard]] auto edit_field(ui::PropertyGridScope& grid, T & val, FieldInfo info) -> bool{
    using namespace std::meta;
    bool edited = false;
    if constexpr (concepts::is_scalar_type<T>){
        grid.scalar_row(info.name_zstring(), info.note_zstring(), [&]{
            edited |= edit_leaf(val, info);
        });
    }else if constexpr (concepts::is_array_type<T>){
        using Et = T::value_type;
        grid.struct_row(info.name_zstring(), info.note_zstring(), std::format("{}",val), [&]{
            auto idx = 0uz;
            for (auto& element: val){
                auto elem_info = info.element_at(idx);
                elem_info.name = std::format("{}[{}]",info.name ? *info.name : ""sv, idx++);
                edited |= edit_field(grid, element, elem_info);
            }
        });
    }else if constexpr (concepts::is_struct_like<T>){
        grid.struct_row(info.name_zstring(), info.note_zstring(), std::format("{}",val), [&]{
            edited |= edit_members_of(grid, val, info);
        });
    }else{
        static_assert(false, mh::join_strings("What the fuk is '",identifier_of(^^T),"'"));
    }
    return edited;
}
template<typename T> requires concepts::is_struct_like<T>
[[nodiscard]] auto edit_members_of(ui::PropertyGridScope& grid, T& val, FieldInfo struct_info) -> bool{
    using namespace std::meta;
    auto name = struct_info.name.value_or("no struct name???"sv);
    bool edited = false;
    bool open = false;
    // a row for a member
    // then if its open, its childrens rows : else its preview.
    template for (constexpr auto m: nonstatic_members_array<T>()){
        using Mt = [:dealias(remove_cvref(type_of(m))):];
        auto info = make_field_info<m>();
        edited |= edit_field(grid,val.[:m:],info);
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
    ig::ShowMetricsWindow();
    auto reg = cfg::get_registration<T>();
    auto const main_window_size = ui::ViewportSize() * 0.85f;
    auto const footer_height = ui::ElementHeight();
    auto res = ConfigActions{};

    auto const main_window_pos = glm::vec2{
        ui::ViewportSize().x - main_window_size.x,
        0.0f,
    };
    auto window_spec = ui::WindowSpec{}
        .set_size(main_window_size)
        .set_pos(main_window_pos)
        .set_title(reg.config_name)
        .lock_size()
    ;
    auto main_content_spec = ui::ChildWindowSpec{}
        .set_title("Srolling Body")
        .set_size({0,-footer_height})
    ;
    auto footer_content_spec = ui::ChildWindowSpec{}
        .set_title("Footer")
        .set_size({0,footer_height})
    ;

    if (auto window = ui::begin_scope(window_spec)){
       // static auto test_flags = ui::TableFlags{ui::TableFlagBits::eNone};
       // ui::edit_flags<ui::TableFlagBits>(test_flags);
        auto & root = storage.live;
        res.edited = false;
        if (auto child = ui::begin_scope(main_content_spec)){
            bool should_collapse = ui::Button( "Collapse all", "Collapses all open trees.");
            auto property_grid_spec = ui::PropertyGridSpec{}
                .set_id(reg.config_name)
                .set_flags(ui::TableFlagBits::eRowBg | ui::TableFlagBits::eBordersInnerV)
            ;
            if (auto grid = ui::begin_scope(property_grid_spec)){
                res.edited = edit_members_of(grid, root, make_field_info<^^T>(true));
            }
        }
        if (auto child = ui::begin_scope(footer_content_spec)){
            ig::Separator();
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
