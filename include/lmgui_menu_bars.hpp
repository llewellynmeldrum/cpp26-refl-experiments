#pragma once 
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <optional>
#include <vector>
#include <cstddef>
#include <utility>

#include "lmgui_macros.hpp"
#include "lmgui_common.hpp"

#include "logger.hpp"
#include "cppslop.hpp"
#include "bitmask_flags.hpp"
#include "zstring_view.hpp"


#if CPP26
namespace ui{
//NOTE: tables extension: PropertyGrid - a two columns table with no header row.
struct MenuBarSpec{
    std::string id{""};
    
    SPEC_SETTER(std::string_view, id);
};

struct MenuBarScope{
    explicit MenuBarScope(MenuBarSpec info)
        : id(info.id)
    {
        isVisible = ImGui::BeginMenuBar();
    }
    ~MenuBarScope(){
        if (isVisible){
            isVisible = false; 
            ImGui::EndMenuBar();
        }
    }
    bool menu_isVisible{};
    auto push_menu(this auto&& m, zstring_view id) -> auto&&{
        m.menu_isVisible = ImGui::BeginMenu(id.cstr());
        return std::forward<decltype(m)>(m);
    }
    auto pop_menu(this auto&& m) -> auto&&{
        if (m.menu_isVisible){
            ImGui::EndMenu();
        }
        return std::forward<decltype(m)>(m);
    }
    template<typename Fn>
    auto menu(this auto&& m, zstring_view label,  Fn&& fn) -> auto&&{
        if (ImGui::BeginMenu(label.cstr())){
            std::invoke(fn);
            ImGui::EndMenu();
        }
        return std::forward<decltype(m)>(m);
    }
    auto item(zstring_view label, std::optional<zstring_view> shortcut) -> bool {
        auto res = ImGui::MenuItem(label.cstr());
        if (shortcut) ui::HintPrevious(*shortcut);
        
        return res;
    }
    NO_MOVE_CTOR(MenuBarScope)
    NO_MOVE_ASSN(MenuBarScope)
    NO_COPY_CTOR(MenuBarScope)
    NO_COPY_ASSN(MenuBarScope)



    operator bool(){ return isVisible; }


    std::string id{""};
    bool isVisible = false;
};
inline auto begin_scope(MenuBarSpec const& spec) -> MenuBarScope{
    return MenuBarScope{spec};
}
} // namespace ui
#endif // cpp26
