#pragma once 

#include <meta>
#include <numeric>
#include <ranges>
#include <optional>

#include <imgui.h>
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
#include "lmgui_menu_bars.hpp"
#include "meta_helpers.hpp"

namespace cfg{

// NOTE: on Leaf
// leaves are nodes which can be represented by a single widget.
// Examples include primitive types (i32, f32, etc), and glm::vec types (glm::vec3, glm::vec4). glm::vec types are yet to be implemented.
template<typename T>
    requires concepts::is_scalar_integral<T> || concepts::is_scalar_floating_point<T>
auto edit_leaf(T& v, FieldInfo info) -> bool{
    auto range = info.range.value_or(Range::default_value<T>());
    bool changed = ui::ClampedSlider<T>(info.ui_label(), v, range.min, range.max, info.input_tooltip());
    return changed;
}

template<typename T>
    requires concepts::is_string_type<T>
auto edit_leaf(T& v, FieldInfo info) -> bool{
    ASSERT_IF_NOT_CLANG(!info.has_range(), "Strings do not support ranges atm. Consider implementing to constrain string length min and max?",v,std::format("{}",info));
    bool changed = ui::InputString(info.ui_label(), v, info.input_tooltip());
    return changed;
}
template<typename T>
    requires same_as_nocvref<T,bool>
auto edit_leaf(T& v, FieldInfo info) -> bool{
    auto range = info.range.value_or(Range::default_value<T>());
    // TODO: if no range, it should probably be a DragFloat
    bool changed = ui::ClampedSlider<T>(info.ui_label(), v, range.min,range.max, info.input_tooltip());
    return changed;
}
template<typename T>
    requires concepts::is_enum<T> 
auto edit_leaf(T& v, FieldInfo info) -> bool{
    // for enums, the useful tooltip is actually the regular one
    return ui::EnumDropdown<T>(info.ui_label(),v, info.tooltip()); 
}

#if CPP26
template<typename T> requires concepts::is_struct_like<T>
[[nodiscard]] auto edit_members_of(ui::PropertyGridScope& grid, T& val, FieldInfo struct_info) -> bool;

template<typename T> 
[[nodiscard]] auto edit_field(ui::PropertyGridScope& grid, T & val, FieldInfo info) -> bool{
    using namespace std::meta;
    bool edited = false;
    if constexpr (concepts::is_scalar_type<T>){
        grid.scalar_row(info.ui_label(), info.tooltip(), info.meta_tooltip(), [&]{
            edited |= edit_leaf(val, info);
        });
    }else if constexpr (concepts::is_array_type<T>){
        using Et = T::value_type;
        grid.struct_row(info.ui_label(), info.tooltip(), info.meta_tooltip(), std::format("{}",val), [&]{
            auto idx = 0uz;
            for (auto& element: val){
                auto elem_info = info.element_at(stable_type_name_zstr<Et>(), idx);
                edited |= edit_field(grid, element, elem_info);
                idx++;
            }
        });
    }else if constexpr (concepts::is_struct_like<T>){
        grid.struct_row(info.ui_label(), info.tooltip(), zstring_view{info.meta_tooltip()}, std::format("{}",val), [&]{
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
    bool edited = false;
    // a row for a member
    // then if its open, its childrens rows : else its preview.
    template for (constexpr auto m: nonstatic_members_array<T>()){
        using Mt = [:dealias(remove_cvref(type_of(m))):];
        auto info = make_field_info<m>(stable_type_name_zstr<Mt>(),false);
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
        .set_title(std::format("'{}' (struct {}, file:{})", reg.config_name, stable_type_name<T>(), reg.header_file_loc))
        .lock_size()
        .set_flags(ui::WindowFlagBits::eMenuBar)
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
        if (auto menu_bar = ui::begin_scope(ui::MenuBarSpec{"Edit"})){
            menu_bar.menu("Edit", [&]{
                res.save_to_disk = menu_bar.item("Save to file",
                    std::format("Saves the current live config state to '{}'.",storage.filename)
                );
                res.load_from_disk = menu_bar.item( "Load from file", 
                    std::format("Loads the config state from '{}'.",storage.filename)
                );
//                ui::Separator();
                res.reset_to_default = menu_bar.item( "Reset to defaults",
                    "Resets all fields to defaults, according to their initializers."
                );
            });
        }
       // static auto test_flags = ui::TableFlags{ui::TableFlagBits::eNone};
       // ui::edit_flags<ui::TableFlagBits>(test_flags);
        auto & root = storage.live;
        res.edited = false;
        if (auto child = ui::begin_scope(main_content_spec)){
            auto property_grid_spec = ui::PropertyGridSpec{}
                .set_id(reg.config_name)
                .set_flags(ui::TableFlagBits::eRowBg | ui::TableFlagBits::eBordersInnerV | ui::TableFlagBits::eBordersOuterH)
            ;
            if (auto grid = ui::begin_scope(property_grid_spec)){
                res.edited = edit_members_of(grid, root, make_field_info<^^T>(stable_type_name_zstr<T>(),true));
            }
        }
        if (auto child = ui::begin_scope(footer_content_spec)){
            ui::Separator();
            ui::Label("This is a footer.");
        }
    }
    return res;
}

} // NOTE: namespace cfg
#endif // CPP26
