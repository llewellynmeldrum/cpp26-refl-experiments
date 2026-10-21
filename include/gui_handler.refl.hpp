#pragma once 

#include <meta>
#include <numeric>

#include "imgui_wrapper.hpp"
#include "cppslop.hpp"
#include "cfg_helpers.hpp"
#include "cfg_helpers.refl.hpp"
#include "cfg_metadata.refl.hpp"
#include "cfg_traits.hpp"
#include "cfg_traits.refl.hpp"
#include "gui_handler.hpp"
#include "field_info.refl.hpp"
#include "meta_helpers.refl.hpp"
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
    auto root_type_name = struct_info.name.value_or("no struct name???"sv);
    bool edited = false;
    if (ig::TreeNode(std::string(root_type_name).c_str())){
        template for (constexpr auto m: nonstatic_members_array<T>()){
            using Mt = [:std::meta::dealias(std::meta::remove_cvref(std::meta::type_of(m))):];
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

template<typename T>
auto edit_config_window(Storage<T> & storage) -> bool{
    auto & root = storage.live;
    bool edited = false;
    if (ig::Begin(std::string(stable_type_name<T>()).c_str())){
        template for (constexpr auto m: nonstatic_members_array<T>()){
            using Mt = [:dealias(remove_cvref(type_of(m))):];
            auto info = make_field_info<m>();
            auto type_name = stable_type_name<Mt>();
            if constexpr (concepts::is_scalar_type<Mt>){
                edited |= edit_leaf(root.[:m:], info);
            }else if constexpr (concepts::is_array_type<Mt>){
                edited |= edit_array_like(root.[:m:], info);
            }else if constexpr (concepts::is_struct_like<Mt>){
                if (ig::TreeNode(std::string(type_name).c_str())){
                    edited |= edit_struct_like(root.[:m:], info);
                    ig::TreePop();
                }
            }
        }
    }
    bool save_to_disk = ig::Button(std::format("Save ('{}')",storage.filename).c_str());
    ui::ApplyTooltip(std::format("Saves the current live config state (in '{}').",storage.filename));
    ig::SameLine();


    bool load_from_disk = ig::Button("Reset to disk");
    ui::ApplyTooltip(std::format("Loads the current disk config state (in '{}').",storage.filename));
    ig::SameLine();


    bool reset_to_default = ig::Button("Reset to defaults");
    ui::ApplyTooltip("Resets all fields to defaults, according to their initializers.");
    ig::SameLine();
    ig::End();
    
    if (reset_to_default){
        storage.live = {};
    }else if (save_to_disk){
        cfg::save_to_disk(storage);
    }else if (load_from_disk){
        cfg::load_from_disk(storage);
    }


    return edited;
}

} // NOTE: namespace cfg
