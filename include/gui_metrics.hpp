#pragma once 
#include <implot.h>

#include "utils.hpp"
#include "gui.hpp"
#include "metrics.hpp"


inline auto display_frametime_graph(MTR::DurationTracker const& trk) -> void{
    auto const& rb = trk.rb;
    using namespace ImPlot;
    if (BeginPlot("Frametime", glm::vec2{800,300}, ImPlotFlags_NoMouseText)){
        SetupAxis(ImAxis_X1, "seconds");
        SetupAxisLimits(ImAxis_X1, trk.x_min(),  trk.x_max(), ImPlotCond_Always);
        SetupAxisFormat(ImAxis_X1, "%gs");

        // TODO: improve the graph. Make it autofit(?), do the thing
        SetupAxis(ImAxis_Y1, "ms");
        SetupAxisLimits(ImAxis_Y1, 0.0, 27.0);
        SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);
        ImPlotSpec trace;
        trace.LineColor = util::_01rgb(0,255,0);
        trace.Stride = rb.stride();
        trace.Offset = rb.offset();

        PlotLine("Frame", trk.get_x_ptr(), trk.get_y_ptr(), trk.count(),trace);


        // TODO: THERE IS BUG HERE
//        auto v_p99 = metrics::p99(trk.rb);
//        ImPlotSpec p99;
//        p99.LineColor = util::_01rgb(255,0,0);
//        p99.Flags = ImPlotInfLinesFlags_Horizontal;
//        ImPlot::PlotInfLines("p99", &v_p99, 1, p99);
//        ImPlot::TagY(v_p99,util::_01rgb(255,0,0) , "p99 %.1f", v_p99);
        EndPlot();
    }
}
