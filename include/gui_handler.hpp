#pragma once 

#include <optional>
//#include "cfg_traits.refl.hpp"
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
