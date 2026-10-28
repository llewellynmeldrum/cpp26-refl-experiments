#pragma once 
#include "lmgui_includes.hpp"
#include "lmgui_macros.hpp"
#include "lmgui_utils.hpp"
namespace ui{
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
    ig::NewLine();
}
inline auto Label(
    std::string_view label,
    std::optional<std::string_view> tooltip=std::nullopt
) -> void{
    ig::TextUnformatted(std::string(label).c_str());
    if (tooltip) ui::InlineTooltip(*tooltip);
}
} // namespace ui
