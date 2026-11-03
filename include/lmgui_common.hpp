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
inline auto TruncatedText(
    zstring_view txt,
    std::size_t num_chars_remaining
) -> void {
    //auto out = txt.clone();
    ASSERT(num_chars_remaining>0);
    auto out = txt.clone().substr(0, std::min(txt.length(), num_chars_remaining));
    if (out.length() != txt.length()){ // truncation
        if (out.length()<3){
            out = "...";
        }else{
            // out.at(out.size()-0) = '.'; the null terminator - i.e past the end of the string. prolly throws std::out_of_range
            out.at(out.size()-1) = '.';
            out.at(out.size()-2) = '.';
            out.at(out.size()-3) = '.';
        }
    }
    ig::TextUnformatted(out.c_str());
}
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
// tooltip with a greyed out suffix
inline auto TwoLineTooltip(zstring_view desc, zstring_view suffix) -> void {
    static constexpr auto second_line_min_length = 35uz;
    ig::SameLine();
    ig::TextDisabled("(?)");
    if (ig::BeginItemTooltip()) {
        ig::TextUnformatted(desc.cstr());
        ImGui::BeginDisabled();
            ui::TruncatedText(suffix,std::max(desc.length(),second_line_min_length));
        ImGui::EndDisabled();
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
inline auto TruncatedText(
    zstring_view txt,
    f32 portion_of_remaining=1.0f
) -> void {
    //auto out = txt.clone();
    ASSERT(portion_of_remaining <= 1.0f);
    auto x_remaining = ImGui::GetContentRegionAvail().x * portion_of_remaining;
    auto font = ImGui::GetFont();
    char const* end{};
    auto* begin = txt.cstr();
    font->CalcTextSizeA(ImGui::GetFontSize(), x_remaining, 0.0f, txt.cstr(), nullptr,&end);
    auto out = std::string(begin, end);
    ASSERT(end!=nullptr);
    if (out.length() == txt.length()){ // no truncation
        out = txt.clone();
    }else{
        if (out.length()<3){
            out = "...";
        }else{
            // out.at(out.size()-0) = '.'; the null terminator - i.e past the end of the string. prolly throws std::out_of_range
            out.at(out.size()-1) = '.';
            out.at(out.size()-2) = '.';
            out.at(out.size()-3) = '.';
        }
    }
    ig::TextUnformatted(out.c_str());
}
inline auto TruncatedPreviewText(
    zstring_view txt,
    std::optional<zstring_view> tooltip=std::nullopt,
    bool grey_tooltip=false
) -> void{
    ig::BeginDisabled();
    TruncatedText(txt,1.0f);
    ig::EndDisabled();
    if (grey_tooltip) ig::BeginDisabled();
    if (tooltip) ui::HintPrevious(*tooltip);
    if (grey_tooltip) ig::EndDisabled();
}
inline auto Separator()->void{
    ImGui::Separator();
}
inline auto Label(
    std::string_view label,
    std::optional<std::string_view> tooltip=std::nullopt
) -> void{
    ig::TextUnformatted(std::string(label).c_str());
    if (tooltip) ui::InlineTooltip(*tooltip);
}
inline auto LabelDisabled(
    std::string_view label,
    std::optional<std::string_view> tooltip=std::nullopt
) -> void{
    ig::BeginDisabled();
    ig::TextUnformatted(std::string(label).c_str());
    if (tooltip) ui::InlineTooltip(*tooltip);
    ig::EndDisabled();
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
