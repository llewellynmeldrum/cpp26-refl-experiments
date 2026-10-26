#pragma once 
#include <ranges>
#include <array>
#include <string>
#include <format>
#include <algorithm>
#include <string_view>

#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_sdlrenderer3.h>
#include <misc/cpp/imgui_stdlib.h>
#include <type_traits>

#include "common.hpp"
#include "zstring_view.hpp"
#include "primitive_types.hpp"
#if CPP26
#include "enum_to_string.hpp"
#endif 
namespace ig = ImGui; //NOLINT
namespace ui{
struct BeginWindowOptions{
    glm::vec2 size{};
};
inline  auto BeginWindow(zstring_view label, BeginWindowOptions opt={}) -> auto {
    if (opt.size != glm::vec2{}){
        ImGui::SetNextWindowSize(opt.size);
    }
    return ImGui::Begin(label.cstr());
}
inline  auto EndWindow() -> auto {
    return ImGui::End();
}
inline  auto BeginChildWindow(zstring_view label, glm::vec2 size) -> auto {
    return ImGui::BeginChild(label.cstr(),size);
}
inline  auto EndChildWindow() -> auto {
    return ImGui::EndChild();
}
[[nodiscard]] inline auto BeginTreeNode(zstring_view label, ImGuiTreeNodeFlags flag={}) -> auto {
    return ig::TreeNodeEx(label.cstr(), flag);
}
inline auto EndTreeNode() -> auto {
    return ig::TreePop();
}

[[nodiscard]] inline  auto ItemSpacing() -> glm::vec2 { return ig::GetStyle().ItemSpacing;}
[[nodiscard]] inline  auto ViewportSize() -> glm::vec2 { return ig::GetMainViewport()->Size;}
[[nodiscard]] inline  auto AvaliableRegion() -> glm::vec2 { return ig::GetContentRegionAvail();}
[[nodiscard]] inline  auto ElementHeight() -> f32{ return ig::GetFrameHeightWithSpacing();}
inline auto InlineTooltip(std::string_view desc) -> void {
    ig::SameLine();
    ig::TextDisabled("(?)");
    if (ig::BeginItemTooltip()) {
        ig::PushTextWrapPos(ig::GetFontSize() * 35.0f);
        ig::TextUnformatted(std::string(desc).c_str());
        ig::PopTextWrapPos();
        ig::EndTooltip();
    }
}
inline auto InlineTooltip(zstring_view desc, bool sameline=true) -> void {
    if (sameline){
        ig::SameLine();
    }
    ig::TextDisabled("(?)");
    if (ig::BeginItemTooltip()) {
        ig::PushTextWrapPos(ig::GetFontSize() * 35.0f);
        ig::TextUnformatted(desc.cstr());
        ig::PopTextWrapPos();
        ig::EndTooltip();
    }
}
inline auto SameLine() -> void {
    ig::SameLine();
}
inline auto HintPrevious(zstring_view desc) -> void {
    ig::SameLine();
    if (ig::BeginItemTooltip()) {
        ig::PushTextWrapPos(ig::GetFontSize() * 35.0f);
        ig::TextUnformatted(desc.cstr());
        ig::PopTextWrapPos();
        ig::EndTooltip();
    }
}
inline auto Button(zstring_view desc, zstring_view hint="", glm::vec2 size={}) -> bool{
    bool res = ig::Button(desc.cstr(), size);
    ui::HintPrevious(hint.cstr());
    return res;
}
inline auto Checkbox(zstring_view label, bool& v, zstring_view hint){
    bool res = ig::Checkbox(label.cstr(), &v);
    ui::InlineTooltip(hint);
}
template<typename T>
inline auto ClampedSlider(
    std::string_view label,
    T& v,
    T min, 
    T max,
    std::optional<std::string_view> tooltip=std::nullopt
) -> bool {
    auto changed = false;
    if constexpr (std::integral<T> && (!std::same_as<T,bool>)){
//        LOG_DBG("t={},min:{},max:{}"sv,stable_type_name<T>(),min,max);
        changed = ig::SliderInt(std::string(label).c_str(),&v,min, max, "%d", ImGuiSliderFlags_ClampOnInput);
    }else {
        changed = ig::SliderFloat(std::string(label).c_str(),&v,min, max, "%.3f", ImGuiSliderFlags_ClampOnInput);
    }
    if (tooltip) ui::InlineTooltip(*tooltip);
    return changed;
}
// TODO: this function needs to be fixed, the inbuf_buf is causing a bunch of null bytes (i think)
// - also the bottom bar with the buttons should be persistent, i.e locked so that its always below. Maybe a separate window?
// - Also i want to use the new default font for imgui which is higher res and much nicer looking
// - 
#include <misc/cpp/imgui_stdlib.h>
inline auto InputString(
    zstring_view label,
    std::string& str,
    std::optional<std::string_view> tooltip=std::nullopt
) -> bool {
    auto changed = ig::InputText(label.cstr(), &str, ImGuiInputTextFlags_CharsNoBlank);
    if (tooltip) ui::InlineTooltip(*tooltip);
    return changed;
}
inline auto Label(
    std::string_view label,
    std::optional<std::string_view> tooltip=std::nullopt
) -> void{
    ig::TextUnformatted(std::string(label).c_str());
    if (tooltip) ui::InlineTooltip(*tooltip);
}
template<typename ...Args>
inline auto LabelF(
    std::format_string<Args...> fmt, 
    Args&& ...args 
) -> void{
    ig::TextUnformatted(std::format(fmt,std::forward<Args>(args)...).c_str());
}

namespace impl{

// SECTION: RAII WRAPPERS
    
struct TreeNodeRaii{
    bool open{false};
    operator bool() const noexcept {return open;}
    TreeNodeRaii(zstring_view label, ImGuiTreeNodeFlags flag={})
        :open(ig::TreeNodeEx(label.cstr(), flag))
    {}
    ~TreeNodeRaii(){ if (open) ig::TreePop(); }
};
struct WindowRaiiWrapper{
    bool open{false};
    operator bool() const noexcept {return open;}
    WindowRaiiWrapper(zstring_view label, BeginWindowOptions opt)
        :open(ui::BeginWindow(label,opt))
    {}
    ~WindowRaiiWrapper(){ui::EndWindow();}
};
struct ChildWindowRaiiWrapper{
    bool open{false};
    operator bool() const noexcept {return open;}
    ChildWindowRaiiWrapper(zstring_view label, glm::vec2 size)
        :open(ui::BeginChildWindow(label,size))
    {}
    ~ChildWindowRaiiWrapper(){ ui::EndChildWindow(); }
};
}
namespace raii{
[[nodiscard]] inline auto Window(zstring_view label, BeginWindowOptions opt={})-> auto {
    return impl::WindowRaiiWrapper{label,opt};
}
[[nodiscard]] inline auto ChildWindow(zstring_view label, glm::vec2 size={}) -> auto {
    return impl::ChildWindowRaiiWrapper{label,size};
}
[[nodiscard]] inline auto TreeNode(zstring_view label, ImGuiTreeNodeFlags flag={}) -> auto {
    return impl::TreeNodeRaii{label,flag};
}
}


#if CPP26
template<typename T> requires std::is_enum_v<T>
inline constexpr auto get_enum_hint_list(){
    using namespace std::meta;
    auto res = std::vector<const char*>{};
    template for (constexpr auto m: define_static_array(enumerators_of(^^T))){
        auto anno = try_get_annotation_of<cfg::Note>(m);
        res.push_back(anno ? (*anno).txt : static_cast<const char*>(nullptr));
    }
    return std::define_static_array(res);
}
template<typename T> requires std::is_enum_v<T>
inline constexpr auto enum_hint_list = get_enum_hint_list<T>();

template<typename T>
    requires std::is_enum_v<T>
inline auto EnumDropdown(
    zstring_view label,
    T& v,
    std::optional<zstring_view> list_hint=std::nullopt
) -> bool {
    static auto selected_idx = index_of_enum_member(v);
    bool changed {};
    auto const& name_list = enum_name_list<T>;
    auto const& values_list = enum_value_list<T>;
    auto const& hint_list = enum_hint_list<T>;
    if (ig::BeginCombo(label.cstr(),name_list.at(index_of_enum_member(v)))){
        for (const auto& [idx,name] : std::views::enumerate(name_list) ){
            auto const is_selected = (selected_idx ==idx);
            if (ig::Selectable(name, is_selected)){
                selected_idx = idx;
                changed = true;
            }
        }
        ig::EndCombo();
    }
    if (changed){
        v = values_list[selected_idx];
    }
    // TODO: how to handle hints? Can we show a hint for the choice + the whole enum itself? How is that supposed to work?
    HintPrevious(hint_list[selected_idx]);
    if (list_hint) InlineTooltip(*list_hint);
    return changed;

}
#endif


}; // NOTE: namespace ui
