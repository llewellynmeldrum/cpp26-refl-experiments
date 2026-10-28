#pragma once 
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <optional>
#include <vector>
#include <cstddef>
#include <utility>

#include "lmgui_macros.hpp"
#include "logger.hpp"
#include "cppslop.hpp"
#include "bitmask_flags.hpp"
#include "zstring_view.hpp"
#include "lmgui_common.hpp"


#if CPP26
namespace ui{
ENUM_WITH_BITWISE TableFlagBits: u32 {
    // Features
    eNone                       = 0,
    eResizable                  = 1 << 0,   // Enable resizing columns.
    eReorderable                = 1 << 1,   // Enable reordering columns in header row. (Need calling TableSetupColumn() + TableHeadersRow() to display headers, or using ContextMenuInBody to access context-menu without headers).
    eHideable                   = 1 << 2,   // Enable hiding/disabling columns in context menu.
    eSortable                   = 1 << 3,   // Enable sorting. Call TableGetSortSpecs() to obtain sort specs. Also see SortMulti and SortTristate.
    eNoSavedSettings            = 1 << 4,   // Disable persisting columns order, width, visibility and sort settings in the .ini file.
    eContextMenuInBody          = 1 << 5,   // Right-click on columns body/contents will also display table context menu. By default it is available in TableHeadersRow().
    // Decorations
    eRowBg                      = 1 << 6,   // Set each RowBg color with ImGuiCol_TableRowBg or ImGuiCol_TableRowBgAlt (equivalent of calling TableSetBgColor with ImGuiTableBgFlags_RowBg0 on each row manually)
    eBordersInnerH              = 1 << 7,   // Draw horizontal borders between rows.
    eBordersOuterH              = 1 << 8,   // Draw horizontal borders at the top and bottom.
    eBordersInnerV              = 1 << 9,   // Draw vertical borders between columns.
    eBordersOuterV              = 1 << 10,  // Draw vertical borders on the left and right sides.
    eBordersH                   = eBordersInnerH | eBordersOuterH, // Draw horizontal borders.
    eBordersV                   = eBordersInnerV | eBordersOuterV, // Draw vertical borders.
    eBordersInner               = eBordersInnerV | eBordersInnerH, // Draw inner borders.
    eBordersOuter               = eBordersOuterV | eBordersOuterH, // Draw outer borders.
    eBorders                    = eBordersInner | eBordersOuter,   // Draw all borders.
    eNoBordersInBody            = 1 << 11,  // [ALPHA] Disable vertical borders in columns Body (borders will always appear in Headers). -> May move to style
    eNoBordersInBodyUntilResize = 1 << 12,  // [ALPHA] Disable vertical borders in columns Body until hovered for resize (borders will always appear in Headers). -> May move to style
    // Sizing Policy (read above for defaults)
    eSizingFixedFit             = 1 << 13,  // Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching contents width.
    eSizingFixedSame            = 2 << 13,  // Columns default to _WidthFixed or _WidthAuto (if resizable or not resizable), matching the maximum contents width of all columns. Implicitly enable NoKeepColumnsVisible.
    eSizingStretchProp          = 3 << 13,  // Columns default to _WidthStretch with default weights proportional to each columns contents widths.
    eSizingStretchSame          = 4 << 13,  // Columns default to _WidthStretch with default weights all equal, unless overridden by TableSetupColumn().
    // Sizing Extra Options
    eNoHostExtendX              = 1 << 16,  // Make outer width auto-fit to columns, overriding outer_size.x value. Only available when ScrollX/ScrollY are disabled and Stretch columns are not used.
    eNoHostExtendY              = 1 << 17,  // Make outer height stop exactly at outer_size.y (prevent auto-extending table past the limit). Only available when ScrollX/ScrollY are disabled. Data below the limit will be clipped and not visible.
    eNoKeepColumnsVisible       = 1 << 18,  // Disable keeping column always minimally visible when ScrollX is off and table gets too small. Not recommended if columns are resizable.
    ePreciseWidths              = 1 << 19,  // Disable distributing remainder width to stretched columns (width allocation on a 100-wide table with 3 columns: Without this flag: 33,33,34. With this flag: 33,33,33). With larger number of columns, resizing will appear to be less smooth.
    // Clipping
    eNoClip                     = 1 << 20,  // Disable clipping rectangle for every individual columns (reduce draw command count, items will be able to overflow into other columns). Generally incompatible with TableSetupScrollFreeze().
    // Padding
    ePadOuterX                  = 1 << 21,  // Default if BordersOuterV is on. Enable outermost padding. Generally desirable if you have headers.
    eNoPadOuterX                = 1 << 22,  // Default if BordersOuterV is off. Disable outermost padding.
    eNoPadInnerX                = 1 << 23,  // Disable inner padding between columns (double inner padding if BordersOuterV is on, single inner padding if BordersOuterV is off).
    // Scrolling
    eScrollX                    = 1 << 24,  // Enable horizontal scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size. Changes default sizing policy. Because this creates a child window, ScrollY is currently generally recommended when using ScrollX.
    eScrollY                    = 1 << 25,  // Enable vertical scrolling. Require 'outer_size' parameter of BeginTable() to specify the container size.
    // Sorting
    eSortMulti                  = 1 << 26,  // Hold shift when clicking headers to sort on multiple column. TableGetSortSpecs() may return specs where (SpecsCount > 1).
    eSortTristate               = 1 << 27,  // Allow no sorting, disable default sorting. TableGetSortSpecs() may return specs where (SpecsCount == 0).
    // Miscellaneous
    eHighlightHoveredColumn     = 1 << 28,  // Highlight column headers when hovered (may evolve into a fuller highlight)
};
using TableFlags = Flags<TableFlagBits>;

// Flags for ImGui::TableSetupColumn()
ENUM_WITH_BITWISE TableColumnFlagBits {
    // Input configuration flags
    eNone                  = 0,
    eDisabled              = 1 << 0,   // Overriding/master disable flag: hide column, won't show in context menu (unlike calling TableSetColumnEnabled() which manipulates the user accessible state)
    eDefaultHide           = 1 << 1,   // Default as a hidden/disabled column.
    eDefaultSort           = 1 << 2,   // Default as a sorting column.
    eWidthStretch          = 1 << 3,   // Column will stretch. Preferable with horizontal scrolling disabled (default if table sizing policy is _SizingStretchSame or _SizingStretchProp).
    eWidthFixed            = 1 << 4,   // Column will not stretch. Preferable with horizontal scrolling enabled (default if table sizing policy is _SizingFixedFit and table is resizable).
    eNoResize              = 1 << 5,   // Disable manual resizing.
    eNoReorder             = 1 << 6,   // Disable manual reordering this column, this will also prevent other columns from crossing over this column.
    eNoHide                = 1 << 7,   // Disable ability to hide/disable this column.
    eNoClip                = 1 << 8,   // Disable clipping for this column (all NoClip columns will render in a same draw command).
    eNoSort                = 1 << 9,   // Disable ability to sort on this field (even if Sortable is set on the table).
    eNoSortAscending       = 1 << 10,  // Disable ability to sort in the ascending direction.
    eNoSortDescending      = 1 << 11,  // Disable ability to sort in the descending direction.
    eNoHeaderLabel         = 1 << 12,  // TableHeadersRow() will submit an empty label for this column. Convenient for some small columns. Name will still appear in context menu or in angled headers. You may append into this cell by calling TableSetColumnIndex() right after the TableHeadersRow() call.
    eNoHeaderWidth         = 1 << 13,  // Disable header text width contribution to automatic column width.
    ePreferSortAscending   = 1 << 14,  // Make the initial sort direction Ascending when first sorting on this column (default).
    ePreferSortDescending  = 1 << 15,  // Make the initial sort direction Descending when first sorting on this column.
    eIndentEnable          = 1 << 16,  // Use current Indent value when entering cell (default for column 0).
    eIndentDisable         = 1 << 17,  // Ignore current Indent value when entering cell (default for columns > 0). Indentation changes _within_ the cell will still be honored.
    eAngledHeader          = 1 << 18,  // TableHeadersRow() will submit an angled header row for this column. Note this will add an extra row.

    // Output status flags, read-only via TableGetColumnFlags()
    eIsEnabled             = 1 << 24,  // Status: is enabled == not hidden by user/api (referred to as "Hide" in _DefaultHide and _NoHide) flags.
    eIsVisible             = 1 << 25,  // Status: is visible == is enabled AND not clipped by scrolling.
    eIsSorted              = 1 << 26,  // Status: is currently part of the sort specs
    eIsHovered             = 1 << 27,  // Status: is hovered by mouse
};
using TableColumnFlags = Flags<TableColumnFlagBits>;

// Flags for ImGui::TableNextRow()
ENUM_WITH_BITWISE TableRowFlagBits{
    eNone                     = 0,
    eHeaders                  = 1 << 0,   // Identify header row (set default background color + width of its contents accounted differently for auto column width)
};
using TableRowFlags = Flags<TableRowFlagBits>;

struct TableColumnHeader{
    std::string title;
    TableColumnFlags flags = TableColumnFlagBits::eNone;
    std::string hint="";
};

struct TableSpec{
    std::string name{""};
    TableFlags flags;
    std::vector<TableColumnHeader> headers{};
    
    SPEC_BUILDER_SET(std::string_view, name);
    SPEC_BUILDER_SET(TableFlags, flags);
    auto&& set_column_headers(this auto&& m, std::vector<TableColumnHeader> headers) {
        m.headers.assign_range(headers);
        return std::forward<decltype(m)>(m);
    }
    auto&& add_column_header(this auto&& m, std::string header, TableColumnFlags col_flags = TableColumnFlagBits::eNone, std::string hint=""){
        m.headers.emplace_back(header,col_flags, hint);
        return std::forward<decltype(m)>(m);
    }
    auto begin();

};
struct TableScope{
    explicit TableScope(TableSpec info)
        : name(info.name)
        , flags(info.flags)
        , headers(info.headers)
    {
        isVisible = ImGui::BeginTable(name.c_str(), headers.size(), flags.raw());
        if (isVisible){
            setup_headers(headers);
        }
    }
    ~TableScope(){
        if (isVisible){
            isVisible = false; 
            ImGui::EndTable();
        }
    }
    NO_MOVE_CTOR(TableScope)
    NO_MOVE_ASSN(TableScope)
    NO_COPY_CTOR(TableScope)
    NO_COPY_ASSN(TableScope)

    operator bool(){ return isVisible; }

    static auto setup_headers(std::span<const TableColumnHeader> headers) ->void;
    auto next_row(this auto&& m, TableRowFlags flags = TableRowFlagBits::eNone) -> auto&&{
        ASSERT(GImGui->CurrentTable != nullptr, "Why is this null");
        ImGui::TableNextRow(flags.raw());
        return std::forward<decltype(m)>(m);
    }
    auto&& next_column(this auto&& m){
        ImGui::TableNextColumn();
        return std::forward<decltype(m)>(m);
    }

    // alternative lambda based API
    template<typename Fn>
    auto&& column(this auto&& m,Fn&& in_column){
        if (ImGui::TableNextColumn()){
            std::invoke(std::forward<Fn>(in_column));
        }
        return std::forward<decltype(m)>(m);
    }

    std::string name{""};
    TableFlags flags;
    bool isVisible = false;
    std::vector<TableColumnHeader> headers{};
};
inline auto TableScope::setup_headers(std::span<const TableColumnHeader> headers) -> void {
    for (const auto &column : headers) {
        ImGui::TableSetupColumn(column.title.c_str(), column.flags.raw());
    }
    // we render our own headers since imgui automatic header shit doesnt allow
    // hints
    for (const auto &column : headers) {
        ImGui::TableNextColumn();
        ImGui::Selectable(column.title.c_str(), true,
                          ImGuiSelectableFlags_AllowOverlap);
        if (column.hint != "") {
            ui::InlineTooltip(zstring_view{column.hint});
        }
    }
}
inline auto begin_scope(TableSpec const& spec) -> TableScope{
    return TableScope{spec};
}

//NOTE: tables extension: PropertyGrid - a two columns table with no header row.
struct PropertyGridSpec{
    std::string id{""};
    TableFlags flags{TableFlagBits::eBorders};
    std::vector<TableColumnHeader> headers{};
    
    SPEC_BUILDER_SET(std::string_view, id);
};

struct PropertyGridScope{
    explicit PropertyGridScope(PropertyGridSpec info)
        : id(info.id)
        ,flags(info.flags)
    {
        isVisible = ImGui::BeginTable(id.c_str(), 2, flags.raw());
    }
    ~PropertyGridScope(){
        if (isVisible){
            isVisible = false; 
            ImGui::EndTable();
        }
    }
    NO_MOVE_CTOR(PropertyGridScope)
    NO_MOVE_ASSN(PropertyGridScope)
    NO_COPY_CTOR(PropertyGridScope)
    NO_COPY_ASSN(PropertyGridScope)

    operator bool(){ return isVisible; }

    template<typename Fn>
    auto&& scalar_row(this auto&& m, std::optional<zstring_view> name, std::optional<zstring_view> hint, Fn&& rhs_column){
        auto row_flags = TableRowFlags{};
        ImGui::TableNextRow(row_flags.raw());
        if (ImGui::TableSetColumnIndex(0)){
            ImGui::AlignTextToFramePadding();
            ImGui::TreeNodeEx(name ? name.value().cstr() : "", ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_Leaf);
            if (hint) ui::InlineTooltip(*hint);
        }
        if (ImGui::TableSetColumnIndex(1)){
            ImGui::SetNextItemWidth(-FLT_MIN);
            ImGui::PushID(name ? name.value().cstr() : "");
            std::invoke(std::forward<Fn>(rhs_column));
            ImGui::PopID();
        }
        return std::forward<decltype(m)>(m);
    }
    template<typename Fn>
    auto&& struct_row(this auto&& m, std::optional<zstring_view> name, std::optional<zstring_view> hint, 
                      Fn&& rhs_column){
        auto row_flags = TableRowFlags{};
        ImGui::TableNextRow(row_flags.raw());
        if (ImGui::TreeNodeEx(name ? name.value().cstr() : "", ImGuiTreeNodeFlags_SpanAllColumns)){
            std::invoke(std::forward<Fn>(rhs_column));
            ImGui::TreePop();
        }else{
            ImGui::Text("PREVIEW...");
        }
        return std::forward<decltype(m)>(m);
    }

    std::string id{""};
    TableFlags flags{};
    bool isVisible = false;
};
inline auto begin_scope(PropertyGridSpec const& spec) -> PropertyGridScope{
    return PropertyGridScope{spec};
}
} // namespace ui
#endif

