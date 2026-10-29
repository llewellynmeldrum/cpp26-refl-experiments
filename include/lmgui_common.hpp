#pragma once 
#include "common.hpp"
#include "lmgui_includes.hpp"
#include "lmgui_macros.hpp"
#include "lmgui_utils.hpp"
#include "logger.hpp"
#include <imgui.h>
#if CPP26
#include <meta>
#endif
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
inline auto TruncatedPreviewText(
    zstring_view txt,
    std::optional<std::string_view> tooltip=std::nullopt
) -> void{
    //auto out = txt.clone();
    auto x_remaining = ImGui::GetContentRegionAvail().x;
    auto font = ImGui::GetFont();
    char const* end{};
    auto* begin = txt.cstr();
    font->CalcTextSizeA(ImGui::GetFontSize(), x_remaining, 0.0f, txt.cstr(), nullptr,&end);
    auto out = std::string(begin, end);
    ASSERT(end!=nullptr);
    if (out.length() == txt.length()){ // no truncation
        out = txt.clone();
    }else{
        if (out.length() != txt.length()){// text got truncated 
            if (out.length()<3){
                out = "...";
            }else{
                // out.at(out.size()-0) = '.'; the null terminator - i.e past the end of the string. prolly throws std::out_of_range
                out.at(out.size()-1) = '.';
                out.at(out.size()-2) = '.';
                out.at(out.size()-3) = '.';
            }
        }
    }
    ig::BeginDisabled();
    ig::TextUnformatted(out.c_str());
    ig::EndDisabled();
    if (tooltip) ui::InlineTooltip(*tooltip);
}
inline auto Label(
    std::string_view label,
    std::optional<std::string_view> tooltip=std::nullopt
) -> void{
    ig::TextUnformatted(std::string(label).c_str());
    if (tooltip) ui::InlineTooltip(*tooltip);
}

template<typename FlagBits, typename Flags> requires std::is_enum_v<FlagBits>
inline auto edit_flags(Flags& v);
#if CPP26
template<typename FlagBits, typename Flags> requires std::is_enum_v<FlagBits>
inline auto edit_flags(Flags& v){
    auto idx = 0uz;
    template for (constexpr auto m : std::define_static_array(std::meta::enumerators_of(^^FlagBits))){
        auto const i = idx++;
        auto e = [:m:];
        if (e == FlagBits::eNone){
            continue;
        }
        auto raw = v.raw();
        auto str = std::format("{:<25}",std::string(std::meta::identifier_of(m)));
        if (ImGui::CheckboxFlags(str.c_str(), &raw,std::to_underlying(e))){
            v = Flags{raw};
        }
        if (i%3!=0){
            ImGui::SameLine();
        }
    }
    ui::Label(std::format("{:032b}",v.raw()));
}
#endif
} // namespace ui
