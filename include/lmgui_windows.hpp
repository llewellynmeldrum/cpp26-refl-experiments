#pragma once 
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <cstddef>
#include <utility>

#include "logger.hpp"
#include "cppslop.hpp"
#include "bitmask_flags.hpp"
#include "zstring_view.hpp"
#include "lmgui_common.hpp"


#if CPP26
namespace ui{
// Flags for ImGui::Begin()
// (Those are per-window flags. There are shared flags in ImGuiIO: io.ConfigWindowsResizeFromEdges and io.ConfigWindowsMoveFromTitleBarOnly)
ENUM_WITH_BITWISE WindowFlagBits{
    eNone                   = 0,
    eNoTitleBar             = 1 << 0,   // Disable title-bar
    eNoResize               = 1 << 1,   // Disable user resizing with the lower-right grip
    eNoMove                 = 1 << 2,   // Disable user moving the window
    eNoScrollbar            = 1 << 3,   // Disable scrollbars (window can still scroll with mouse or programmatically)
    eNoScrollWithMouse      = 1 << 4,   // Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
    eNoCollapse             = 1 << 5,   // Disable user collapsing window by double-clicking on it. Also referred to as Window Menu Button (e.g. within a docking node).
    eAlwaysAutoResize       = 1 << 6,   // Resize every window to its content every frame
    eNoBackground           = 1 << 7,   // Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
    eNoSavedSettings        = 1 << 8,   // Never load/save settings in .ini file
    eNoMouseInputs          = 1 << 9,   // Disable catching mouse, hovering test with pass through.
    eMenuBar                = 1 << 10,  // Has a menu-bar
    eHorizontalScrollbar    = 1 << 11,  // Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
    eNoFocusOnAppearing     = 1 << 12,  // Disable taking focus when transitioning from hidden to visible state
    eNoBringToFrontOnFocus  = 1 << 13,  // Disable bringing window to front when taking focus (e.g. clicking on it or programmatically giving it focus)
    eAlwaysVerticalScrollbar= 1 << 14,  // Always show vertical scrollbar (even if ContentSize.y < Size.y)
    eAlwaysHorizontalScrollbar=1<< 15,  // Always show horizontal scrollbar (even if ContentSize.x < Size.x)
    eNoNavInputs            = 1 << 16,  // No keyboard/gamepad navigation within the window
    eNoNavFocus             = 1 << 17,  // No focusing toward this window with keyboard/gamepad navigation (e.g. skipped by Ctrl+Tab)
    eUnsavedDocument        = 1 << 18,  // Display a dot next to the title. When used in a tab/docking context, tab is selected when clicking the X + closure is not assumed (will wait for user to stop submitting the tab). Otherwise closure is assumed when pressing the X, so if you keep submitting the tab may reappear at end of tab bar.
    eNoDocking              = 1 << 19,  // Disable docking of this window
    eNoNav                  = eNoNavInputs | eNoNavFocus,
    eNoDecoration           = eNoTitleBar | eNoResize | eNoScrollbar | eNoCollapse,
    eNoInputs               = eNoMouseInputs | eNoNavInputs | eNoNavFocus,

    // [Internal]
    eDockNodeHost           = 1 << 23,  // Don't use! For internal use by Begin()/NewFrame()
    eChildWindow            = 1 << 24,  // Don't use! For internal use by BeginChild()
    eTooltip                = 1 << 25,  // Don't use! For internal use by BeginTooltip()
    ePopup                  = 1 << 26,  // Don't use! For internal use by BeginPopup()
    eModal                  = 1 << 27,  // Don't use! For internal use by BeginPopupModal()
    eChildMenu              = 1 << 28,  // Don't use! For internal use by BeginMenu()

};
using WindowFlags = Flags<WindowFlagBits>;



struct WindowSpec{
    std::string title{""};
    WindowFlags flags{};
    glm::vec2 pos{};
    glm::vec2 size{};
    glm::vec2 pivot{};

    SPEC_ENABLE_FLAG(disable_docking, WindowFlagBits::eNoDocking);
    SPEC_ENABLE_FLAG(lock_size, WindowFlagBits::eNoResize);
    SPEC_ENABLE_FLAG(lock_pos, WindowFlagBits::eNoMove);

    SPEC_SETTER(std::string_view, title);
    SPEC_SETTER(WindowFlags, flags);
    SPEC_SETTER(glm::vec2, pos);
    SPEC_SETTER(glm::vec2, size);
    SPEC_SETTER(glm::vec2, pivot);
};

struct WindowScope{
    explicit WindowScope(WindowSpec info)
        : title(info.title)
        , flags(info.flags)
    {
        auto pos_condition = flags.has(WindowFlagBits::eNoMove) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        auto size_condition = flags.has(WindowFlagBits::eNoResize) ? ImGuiCond_Always : ImGuiCond_FirstUseEver;
        ImGui::SetNextWindowPos(info.pos,pos_condition,info.pivot);
        ImGui::SetNextWindowSize(info.size,size_condition);
        isVisible = ImGui::Begin(title.c_str(), nullptr, flags.raw());
    }
    ~WindowScope(){
        // unconditional end() call, unique to ImGui::Begin() and ImGui::BeginChild()
        ImGui::End();
    }
    NO_MOVE_CTOR(WindowScope)
    NO_MOVE_ASSN(WindowScope)
    NO_COPY_CTOR(WindowScope)
    NO_COPY_ASSN(WindowScope)

    operator bool(){ return isVisible; }

    std::string title{""};
    WindowFlags flags{};
    bool isVisible;
};

inline auto begin_scope(WindowSpec const& spec) -> WindowScope{
    return WindowScope{spec};
}


// NOTE: arguably everything below could be lmgui_child.hpp but thats kind of a creepy title lmao
// Flags for ImGui::BeginChild()
// (Legacy: bit 0 must always correspond to ImGuiChildFlags_Borders to be backward compatible with old API using 'bool border = false'.)
// About using AutoResizeX/AutoResizeY flags:
// - May be combined with SetNextWindowSizeConstraints() to set a min/max size for each axis (see "Demo->Child->Auto-resize with Constraints").
// - Size measurement for a given axis is only performed when the child window is within visible boundaries, or is just appearing.
//   - This allows BeginChild() to return false when not within boundaries (e.g. when scrolling), which is more optimal. BUT it won't update its auto-size while clipped.
//     While not perfect, it is a better default behavior as the always-on performance gain is more valuable than the occasional "resizing after becoming visible again" glitch.
//   - You may also use ImGuiChildFlags_AlwaysAutoResize to force an update even when child window is not in view.
//     HOWEVER PLEASE UNDERSTAND THAT DOING SO WILL PREVENT BeginChild() FROM EVER RETURNING FALSE, disabling benefits of coarse clipping.
ENUM_WITH_BITWISE ChildWindowFlagBits{
    eNone                    = 0,
    eBorders                 = 1 << 0,   // Show an outer border and enable WindowPadding. (IMPORTANT: this is always == 1 == true for legacy reason)
    eAlwaysUseWindowPadding  = 1 << 1,   // Pad with style.WindowPadding even if no border are drawn (no padding by default for non-bordered child windows because it makes more sense)
    eResizeX                 = 1 << 2,   // Allow resize from right border (layout direction). Enable .ini saving (unless ImGuiWindowFlags_NoSavedSettings passed to window flags)
    eResizeY                 = 1 << 3,   // Allow resize from bottom border (layout direction). "
    eAutoResizeX             = 1 << 4,   // Enable auto-resizing width. Read "IMPORTANT: Size measurement" details above.
    eAutoResizeY             = 1 << 5,   // Enable auto-resizing height. Read "IMPORTANT: Size measurement" details above.
    eAlwaysAutoResize        = 1 << 6,   // Combined with AutoResizeX/AutoResizeY. Always measure size even when child is hidden, always return true, always disable clipping optimization! NOT RECOMMENDED.
    eFrameStyle              = 1 << 7,   // Style the child window like a framed item: use FrameBg, FrameRounding, FrameBorderSize, FramePadding instead of ChildBg, ChildRounding, ChildBorderSize, WindowPadding.


    eNavFlattened            = 1 << 8,   // [BETA] Share focus scope, allow keyboard/gamepad navigation to cross over parent border to this child or between sibling child windows.

};
using ChildWindowFlags = Flags<ChildWindowFlagBits>;

struct ChildWindowSpec{
    std::string title{""};
    ChildWindowFlags flags{};
    glm::vec2 size{};

    SPEC_SETTER(std::string_view, title);
    SPEC_SETTER(ChildWindowFlags, flags);
    SPEC_SETTER(glm::vec2, pos);
    SPEC_SETTER(glm::vec2, size);
    SPEC_SETTER(glm::vec2, pivot);
};

struct ChildWindowScope{
    explicit ChildWindowScope(ChildWindowSpec info)
        : title(info.title)
        , flags(info.flags)
    {
        isVisible = ImGui::BeginChild(title.c_str(), info.size, flags.raw());
    }
    //NOTE:unconditional end() call, unique to ImGui::Begin() and ImGui::BeginChild()
    ~ChildWindowScope(){
        ImGui::EndChild();
    }
    NO_MOVE_CTOR(ChildWindowScope)
    NO_MOVE_ASSN(ChildWindowScope)
    NO_COPY_CTOR(ChildWindowScope)
    NO_COPY_ASSN(ChildWindowScope)

    operator bool(){ return isVisible; }

    std::string title{""};
    ChildWindowFlags flags{};
    bool isVisible;
};

inline auto begin_scope(ChildWindowSpec const& spec) -> ChildWindowScope{
    return ChildWindowScope{spec};
}
}// namespace ui
#endif 

