#pragma once 
#include <implot.h>
#include <map>

#include "common.hpp"
#include "glm/detail/qualifier.hpp"
#include "utils.hpp"
#include "gui.hpp"
#include "metrics.hpp"

#if CPP26
#include "enum_to_string.hpp"
#endif //CPP26

template<typename T>
inline auto ImVec(T const& v){
    if constexpr(std::same_as<T,glm::vec2>){
        return ImVec2(v.x,v.y);
    }else if constexpr(std::same_as<T,glm::vec4>){
        return ImVec4(v.x,v.y,v.z,v.w);
    }else{
        throw std::logic_error("Vector has no ImVec equivalent.");
    }
}
inline auto display_frametime_graph(
    u64 frame_count,
    std::vector<TrackerStage> trackers
) -> void{
    using namespace ImPlot;
    if (BeginPlot("Frametime", glm::vec2{800,300}, ImPlotFlags_NoMouseText)){
        SetupAxis(ImAxis_X1, "frames");
        auto& stage0 = trackers[1];
        auto& hbuf = stage0.tracker.hbuf;
        // the max X is the X coordinate of the newest sample
        auto max_x = f64(hbuf.write_count);
        // the min X is the X coordinate of the oldest sample
        auto min_x = f64(hbuf.write_count) - f64(stage0.tracker.Capacity);
        SetupAxisLimits(ImAxis_X1, min_x, max_x, ImPlotCond_Always);
        SetupAxisFormat(ImAxis_X1, "");


        SetupAxis(ImAxis_Y1, "ms");
        SetupAxisLimits(ImAxis_Y1, 0.0, 50.0);
        SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);
        auto graph_x_start = f64(hbuf.total_samples()) - f64(hbuf.valid_samples());
        PlotShaded(stage0.name.cstr(), hbuf.data(), hbuf.valid_samples(), 0, 1, graph_x_start, {
            ImPlotProp_FillColor, ImVec(stage0.gr_fill),
            ImPlotProp_Stride, hbuf.stride(),
            ImPlotProp_Offset , hbuf.write_slot(),
            ImPlotProp_FillAlpha, 0.5f,
        });
        PlotLine(stage0.name.cstr(), hbuf.data(), hbuf.valid_samples(), 1, graph_x_start,{
            ImPlotProp_LineColor, ImVec(stage0.gr_line),
            ImPlotProp_Stride, hbuf.stride(),
            ImPlotProp_Offset , hbuf.write_slot(),
        });



        EndPlot();
    }
}
