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

#include "cfg_metadata.hpp"
#include "cfg_traits.hpp"
#include "stable_type_name.hpp"
#include "primitive_types.hpp"
namespace ig = ImGui; //NOLINT
namespace ui{
[[nodiscard]] inline  auto ItemSpacing() -> auto { return ig::GetStyle().ItemSpacing;}
[[nodiscard]] inline  auto AvaliableRegion() -> auto { return ig::GetContentRegionAvail();}
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
// TODO: I want to make a cstring_view type, which is basically just a string view that guarnt
//constexpr auto cstr(std::string_view sv)-> char const*{ return std::string().c_str();}
//inline auto Button(std::string_view desc, std::string_view hint="") -> void {
//    ig::Button(cstr(desc))
//    ui::Hint("Resets all fields to defaults, according to their initializers.");
//}
inline auto Hint(std::string_view desc) -> void {
    ig::SameLine();
    if (ig::BeginItemTooltip()) {
        ig::PushTextWrapPos(ig::GetFontSize() * 35.0f);
        ig::TextUnformatted(std::string(desc).c_str());
        ig::PopTextWrapPos();
        ig::EndTooltip();
    }
}
template<typename T>
inline auto ClampedSlider(
    std::string_view label,
    T& v,
    cfg::Range range,
    std::optional<std::string_view> tooltip=std::nullopt
) -> bool {
    auto changed = false;
    if constexpr (cfg::concepts::is_scalar_integral<T>){
        LOG_DBG("t={},min:{},max:{}"sv,stable_type_name<T>(),range.min,range.max);
        changed = ig::SliderInt(std::string(label).c_str(),&v,range.min, range.max, "%d", ImGuiSliderFlags_ClampOnInput);
    }else {
        changed = ig::SliderFloat(std::string(label).c_str(),&v,range.min, range.max, "%.3f", ImGuiSliderFlags_ClampOnInput);
    }
    if (tooltip) ui::InlineTooltip(*tooltip);
    return changed;
}
// TODO: this function needs to be fixed, the inbuf_buf is causing a bunch of null bytes (i think)
// - also the bottom bar with the buttons should be persistent, i.e locked so that its always below. Maybe a separate window?
// - Also i want to use the new default font for imgui which is higher res and much nicer looking
// - 
inline auto InputString(
    std::string_view label,
    std::string& str,
    std::optional<std::string_view> tooltip=std::nullopt
) -> bool {
    static constexpr auto MaxInputStringSize = 128uz;
    ASSERT(str.size() <= MaxInputStringSize, "String exceeds the length for ui::InputValidString.",str);
    auto input_buf = std::vector<char>(std::from_range, str);
    input_buf.resize(MaxInputStringSize);
    auto changed = ig::InputText(std::string(label).c_str(), input_buf.data(), input_buf.size(), ImGuiInputTextFlags_CharsNoBlank);
    // locate the first null byte and resize
    str.assign_range(input_buf);
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
    ig::TextUnformatted(std::format(fmt,args...).c_str());
}
}; // NOTE: namespace ui
