#pragma once 
#include <imgui.h>
#include <imgui_internal.h>
#include <string>
#include <vector>
#include <cstddef>
#include "bitmask_flags.hpp"
#include "logger.hpp"

#include "lmgui_general.hpp"

namespace ui{
enum struct TableFlagBits: u32 {
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
enum struct TableColumnFlagBits {
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
enum  struct TableRowFlagBits{
    eNone                     = 0,
    eHeaders                  = 1 << 0,   // Identify header row (set default background color + width of its contents accounted differently for auto column width)
};
using TableRowFlags = Flags<TableRowFlagBits>;

// Enum for ImGui::TableSetBgColor()
// Background colors are rendering in 3 layers:
//  - Layer 0: draw with RowBg0 color if set, otherwise draw with ColumnBg0 if set.
//  - Layer 1: draw with RowBg1 color if set, otherwise draw with ColumnBg1 if set.
//  - Layer 2: draw with CellBg color if set.
// The purpose of the two row/columns layers is to let you decide if a background color change should override or blend with the existing color.
// When using RowBg on the table, each row has the RowBg0 color automatically set for odd/even rows.
// If you set the color of RowBg0 target, your color will override the existing RowBg0 color.
// If you set the color of RowBg1 or ColumnBg1 target, your color will blend over the RowBg0 color.
enum ImGuiTableBgTarget_
{
    ImGuiTableBgTarget_None                     = 0,
    ImGuiTableBgTarget_RowBg0                   = 1,        // Set row background color 0 (generally used for background, automatically set when RowBg is used)
    ImGuiTableBgTarget_RowBg1                   = 2,        // Set row background color 1 (generally used for selection marking)
    ImGuiTableBgTarget_CellBg                   = 3,        // Set cell background color (top-most color)
};

struct TableColumnHeader{
    std::string title;
    TableColumnFlags flags = TableColumnFlagBits::eNone;
    std::string hint="";
};

// NOTE: dtor calls table.end();
struct Table{
    Table() = default;
    Table(std::string name, TableFlags flags)
        :m_name(std::move(name))
        ,m_flags(flags)
    {}
    ~Table(){
        //LOG_TRACE();
        if (m_isShown){end();}
    }

    auto&& set_name(this auto&& self, std::string_view name){
        self.m_name = name;
        return std::forward<decltype(self)>(self);
    }
    auto&& set_flags(this auto&& self, TableFlags flags){
        self.m_flags = flags;
        return std::forward<decltype(self)>(self);
    }
    auto&& set_column_headers(this auto&& self, std::vector<TableColumnHeader> headers) {
        self.m_headers.assign_range(headers);
        return std::forward<decltype(self)>(self);
    }
    auto&& add_column_header(this auto&& self, std::string header, TableColumnFlags col_flags = TableColumnFlagBits::eNone, std::string hint=""){
        self.m_headers.emplace_back(header,col_flags, hint);
        return std::forward<decltype(self)>(self);
    }
    auto begin(this Table&& self)=delete("Begin cannot be called on a temporary/rvalue Table.");
    auto begin(this Table& self) -> Table& {
        ASSERT(self.m_headers.size()>0);
        ASSERT(!self.m_isShown,"Double begin. ");
        self.m_isShown = ImGui::BeginTable(self.m_name.c_str(), self.m_headers.size(), self.m_flags.raw());
        for (const auto& col: self.m_headers){
            ImGui::TableSetupColumn(col.title.c_str(),col.flags.raw());
        }
        // we render our own headers since imgui automatic header shit doesnt allow hints
        for (const auto& col: self.m_headers){
            ImGui::TableNextColumn();
            ImGui::Selectable(col.title.c_str(), true, ImGuiSelectableFlags_AllowOverlap);
            if (col.hint != ""){
                ui::InlineTooltip(zstring_view{col.hint});
            }
        }
        return self;
    }
    auto&& next_row(this auto&& self, TableRowFlags flags = TableRowFlagBits::eNone){
        ASSERT(GImGui->CurrentTable != nullptr, "Why is this null");
        ImGui::TableNextRow(flags.raw());
        return std::forward<decltype(self)>(self);
    }
    auto&& next_column(this auto&& self){
        ImGui::TableNextColumn();
        return std::forward<decltype(self)>(self);
    }

    // alternative lambda based API
    template<typename Fn>
    auto&& column(this auto&& self,Fn&& in_column){
        if (ImGui::TableNextColumn()){
            std::invoke(std::forward<Fn>(in_column));
        }
        return std::forward<decltype(self)>(self);
    }
    auto is_shown(this auto&& self) -> bool{
        return self.m_isShown; 
    }
    auto end() -> void{
        m_isShown = false; 
        ImGui::EndTable();
    }
    std::string m_name{""};
    TableFlags m_flags;
    bool m_isShown = false;
    std::vector<TableColumnHeader> m_headers;
};
}
// NOTE: This is why annotations are superior. We need to escape the namespace 
ENABLE_ENUM_BITWISE(ui::TableRowFlagBits);
ENABLE_ENUM_BITWISE(ui::TableColumnFlagBits);
ENABLE_ENUM_BITWISE(ui::TableFlagBits);
