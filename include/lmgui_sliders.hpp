#pragma once 
#include <concepts>
#include <format>
#include <meta>
#include <numeric>
#include <ranges>
#include <optional>

#include <libassert/assert.hpp>
#include <imgui.h>

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

#include "zstring_view.hpp"
#include "meta_helpers.hpp"

#include "lmgui_common.hpp"

namespace ui{
template<typename T> requires std::floating_point<T> || (std::integral<T> && (!std::same_as<T,bool>))
inline auto ClampedSlider(
    zstring_view label,
    T& v, T min, T max,
    std::optional<zstring_view> opt_tooltip = std::nullopt
) -> bool {
    auto changed = false;
    if constexpr (std::floating_point<T>){
        changed = ImGui::SliderFloat(label.cstr(), &v,min, max, "%.3f", ImGuiSliderFlags_ClampOnInput);
    }else {
        changed = ImGui::SliderInt(label.cstr(), &v,min, max, "%d", ImGuiSliderFlags_ClampOnInput);
    }
    if (opt_tooltip) ui::HintPrevious(*opt_tooltip);
    return changed;
}
}
