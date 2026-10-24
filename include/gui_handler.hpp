#pragma once 

#include <optional>
#include "imgui_wrapper.hpp"
#include "cfg_traits.hpp"
#include "field_info.hpp"
#include "stable_type_name.hpp"

namespace cfg{
template<typename T>
auto edit_leaf(T& v) -> bool {
    static_assert(false, "edit_leaf() is unimplemented for T=" + stable_type_name<T>());
    return false;
}

// NOTE: on Leaf
// leaves are nodes which can be represented by a single widget.
// Examples include primitive types (i32, f32, etc), and glm::vec types (glm::vec3, glm::vec4). glm::vec types are yet to be implemented.
template<typename T>
    requires concepts::is_scalar_integral<T> || concepts::is_scalar_floating_point<T>
auto edit_leaf(T& v, FieldInfo info) -> bool{
    auto label = info.get_label();
    auto range = info.range.value_or(Range::default_value<T>());
    bool changed = ui::ClampedSlider<T>(label, v, range, info.tooltip());
    return changed;
}

template<typename T>
    requires concepts::is_string_type<T>
auto edit_leaf(T& v, FieldInfo info) -> bool{
    auto label = info.get_label();
    ASSERT(!info.has_range(), "Strings do not support ranges atm. Consider implementing to constrain string length min and max?");
    bool changed = ui::InputString(label, v, info.tooltip());
    return changed;
}




} // NOTE: namespace cfg

#include "common.hpp"
#if CPP26
// --- formerly gui_handler.refl.hpp ---
#include <meta>
#include <numeric>

#include "cppslop.hpp"
#include "cfg_helpers.hpp"
#include "cfg_metadata.hpp"
#include "toml_traits.hpp"
#include "meta_helpers.hpp"
#include "cfg_toml_rw.hpp"


//
namespace cfg{
template<typename T>
struct GuiHandler{
    GuiHandler() = delete;
    auto edit(T& v) -> bool;
};

template<typename T> requires concepts::is_array_type<T>
auto edit_array_like(T& arr, FieldInfo info) -> bool{
    bool changed = false;
    using elem_t = element_t<T>;
    static_assert(concepts::is_scalar_type<elem_t> || concepts::struct_of_scalars<elem_t>,
        "Elements of arrays must be either scalar, or structs containing only scalars.");
    auto ui_id = std::string("##"+ std::string(stable_type_name<T>()));
    auto array_name = info.name.value_or("unnamed array??");
    auto flags = ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg;
    ui::Label(array_name, info.tooltip());
    if constexpr(concepts::is_scalar_type<elem_t>){
        // arrays of scalars -> simple table
        if (ig::BeginTable(ui_id.c_str(), 2, flags)){
            ig::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed);
            ig::TableSetupColumn("Value", ImGuiTableColumnFlags_WidthStretch);
            ig::TableHeadersRow();
            auto idx = 0uz;
            for (auto& v:arr){
                ig::TableNextRow();
                ig::TableSetColumnIndex(0);
                ui::Label(std::format("[{}]",idx++));
                ig::TableSetColumnIndex(1);
                changed |= edit_leaf(v, info.element_of(idx));
            }
            ig::EndTable();
        }
    }else{
        // arrays of structs containing only scalars
        ui::Label("IMAGINE THERE WAS A TABLE HERE");
    }
    return changed;
}
template<typename T> requires concepts::is_struct_like<T>
auto edit_struct_like(T& val, FieldInfo struct_info) -> bool{
    using namespace std::meta;
    auto root_type_name = struct_info.name.value_or("no struct name???"sv);
    bool edited = false;
    if (ig::TreeNode(std::string(root_type_name).c_str())){
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
        ig::TreePop();
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
    auto & root = storage.live;
    bool edited = false;
    ig::ShowMetricsWindow();
    auto const footer_height = ui::ElementHeight();
    auto const content_height = ui::AvaliableRegion().y - footer_height - ui::ItemSpacing().y;
    if (ig::BeginChild("ScrollingBody", {0.0f,content_height})){
        template for (constexpr auto m: nonstatic_members_array<T>()){
            using Mt = [:dealias(remove_cvref(type_of(m))):];
            auto info = make_field_info<m>();
            if constexpr (concepts::is_scalar_type<Mt>){
                edited |= edit_leaf(root.[:m:], info);
            }else if constexpr (concepts::is_array_type<Mt>){
                edited |= edit_array_like(root.[:m:], info);
            }else if constexpr (concepts::is_struct_like<Mt>){
                if (ig::TreeNode(std::string(stable_type_name<Mt>()).c_str())){
                    edited |= edit_struct_like(root.[:m:], info);
                    ig::TreePop();
                }
            }
        }
    }
    ig::Separator();
    ig::EndChild();

    bool save_to_disk{}, load_from_disk{}, reset_to_default{};
    if (ig::BeginChild("##",ImVec2(0,footer_height),0,0)){
        ig::Separator();
        save_to_disk = ig::Button(std::format("Save to file",storage.filename).c_str());
        ui::Hint(std::format("Saves the current live config state to '{}'.",storage.filename));
        ig::SameLine();


        load_from_disk = ig::Button("Load from file");
        ui::Hint(std::format("Loads the config state from '{}'.",storage.filename));
        ig::SameLine();


        reset_to_default = ig::Button("Reset to defaults");
        ui::Hint("Resets all fields to defaults, according to their initializers.");
        ig::SameLine();
    }
    ig::EndChild();
    return {
        .edited           = edited,
        .save_to_disk     = save_to_disk,
        .load_from_disk   = load_from_disk,
        .reset_to_default = reset_to_default,
    };
}

} // NOTE: namespace cfg
#endif // CPP26
