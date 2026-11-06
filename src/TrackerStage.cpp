#include <implot.h>
#include <implot_internal.h>
#include "gui.hpp"

auto TrackerStage::check_enabled() noexcept -> bool{
    auto* item = ImPlot::GetItem(name.cstr());
    if (!item || item->Show){
        return was_enabled_this_frame = true;
    }else{
        return was_enabled_this_frame = false;
    }
}
auto TrackerStage::im_id() const noexcept -> std::string{
    return std::format("{}",name);
}
