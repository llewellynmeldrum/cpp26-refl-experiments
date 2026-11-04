#pragma once 
#include <implot.h>
#include <map>

#include "common.hpp"
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
    zstring_view title,
    TrackerStage frametime_tracker
) -> void{
    using namespace ImPlot;
    if (BeginPlot(title.cstr(), glm::vec2{800,300}, ImPlotFlags_NoMouseText)){
        SetupAxis(ImAxis_X1, "frames");
        auto& hbuf = frametime_tracker.tracker.hbuf;
        // the max X is the X coordinate of the newest sample
        auto max_x = f64(hbuf.total_samples())-1.0;
        // the min X is the X coordinate of the oldest sample
        auto min_x = f64(hbuf.total_samples()) - f64(hbuf.Capacity);
        SetupAxisLimits(ImAxis_X1, min_x, max_x, ImPlotCond_Always);
        SetupAxisFormat(ImAxis_X1, "");


        SetupAxis(ImAxis_Y1, "ms");
        SetupAxisLimits(ImAxis_Y1, 0.0, 50.0);
        SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);
        auto graph_x_start = f64(hbuf.total_samples()) - f64(hbuf.valid_samples());

        PlotShaded(frametime_tracker.name.cstr(), hbuf.data(), hbuf.valid_samples(), 0, 1, graph_x_start, {
            ImPlotProp_FillColor, ImVec(frametime_tracker.gr_fill),
            ImPlotProp_FillAlpha, 0.5f,
            ImPlotProp_Stride, hbuf.stride(),
            ImPlotProp_Offset , hbuf.slot_of(0),
            
        });
        PlotLine(frametime_tracker.name.cstr(), hbuf.data(), hbuf.valid_samples(), 1, graph_x_start,{
            ImPlotProp_LineColor, ImVec(frametime_tracker.gr_line),
            ImPlotProp_Stride, hbuf.stride(),
            ImPlotProp_Offset , hbuf.slot_of(0),
        });



        EndPlot();
    }
}
inline auto display_stacked_frametime_breakdown_graph(
    zstring_view title,
    std::vector<TrackerStage> stages
) -> void{
    using namespace ImPlot;
    u64 oldest_shared_sample = 0uz; // oldest shared sample between all stages
    u64 newest_shared_sample = std::numeric_limits<u64>::max(); // newest shared sample between all stages
    for (const auto& stage: stages){
        auto const& hbuf = stage.tracker.hbuf;
        oldest_shared_sample = std::max(oldest_shared_sample, hbuf.oldest_sample());
        newest_shared_sample = std::min(newest_shared_sample, hbuf.total_samples());
    }
    auto num_samples = newest_shared_sample > oldest_shared_sample ? newest_shared_sample - oldest_shared_sample : 0;
    // we also need to make the xs, which will be oldest_sample -> newest_sample
    auto xs = std::vector(num_samples, f64{});
    for (auto idx = 0uz; idx < num_samples; idx++){ // might need to be idx<num_samples-1
        xs.at(idx) = oldest_shared_sample + idx;
    }
    auto stage_y_stop = std::vector(stages.size(), std::vector(num_samples, f64{}));
    auto stage_y_start = std::vector(stages.size(), std::vector(num_samples, f64{}));
    auto y_offset_cumulative = std::vector(num_samples, f64{});
    auto xstart = newest_shared_sample - num_samples;
    // TODO: make ystart and ystop. For now i think this is just ystop
    for (auto const& [stage_idx, stage] : std::views::enumerate(stages)){
        auto const& hbuf = stage.tracker.hbuf;
        for (auto idx = 0uz; idx < num_samples; idx++){ // might need to be idx<num_samples-1
            auto& y0 = stage_y_start.at(stage_idx);
            auto& y1 = stage_y_stop.at(stage_idx);

            auto stage_sample = hbuf.sample(oldest_shared_sample + idx);

            y0.at(idx) = y_offset_cumulative.at(idx);
            y1.at(idx) = stage_sample;
            y_offset_cumulative.at(idx) += stage_sample; 
        }
    }
    if (BeginPlot(title.cstr(), glm::vec2{800,300}, ImPlotFlags_NoMouseText)){
        SetupAxis(ImAxis_X1, "frames");
        // the max X is the X coordinate of the newest sample
        auto max_x = newest_shared_sample;
        // the min X is the X coordinate of the oldest sample
        auto min_x = newest_shared_sample - num_samples;
        SetupAxisLimits(ImAxis_X1, min_x, max_x, ImPlotCond_Always);
        SetupAxisFormat(ImAxis_X1, "");

        SetupAxis(ImAxis_Y1, "ms");
        SetupAxisLimits(ImAxis_Y1, 0.0, 25.0);
        SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);
        //PlotShaded(
        //  const char* label_id, 
        //  const T* xs, 
        //  const T* ys1, 
        //  const T* ys2, 
        //  int count, 
        //  const ImPlotSpec& spec
        // )
        for (auto const& [stage_idx, stage] : std::views::enumerate(stages)){
            auto const& hbuf = stage.tracker.hbuf;
            auto const& y0 = stage_y_start[stage_idx];
            auto const& y1 = stage_y_stop[stage_idx];
//            PlotLine(stage.name.cstr(), xs.data(), y1.data(), y1.size(), {
//                ImPlotProp_LineColor, ImVec(stage.gr_line),
//                ImPlotProp_Stride, hbuf.stride(),
//                ImPlotProp_Offset , hbuf.slot_of(0),
//            });
            ASSERT(xs.size() == y0.size() && y0.size() == y1.size());
            PlotShaded(stage.name.cstr(), xs.data(), y0.data(), y1.data(), y1.size(),{
                ImPlotProp_FillColor, ImVec(stage.gr_fill),
                ImPlotProp_FillAlpha, 0.1f,
                ImPlotProp_Stride, hbuf.stride(),
                ImPlotProp_Offset , hbuf.slot_of(0),
            });
        }



//        PlotShaded(stage0.name.cstr(), hbuf.data(), hbuf.valid_samples(), 0, 1, graph_x_start, {
//            ImPlotProp_FillColor, ImVec(stage0.gr_fill),
//            ImPlotProp_FillAlpha, 0.5f,
//            ImPlotProp_Stride, hbuf.stride(),
//            ImPlotProp_Offset , hbuf.slot_of(0),
//            
//        });
//        PlotLine(stage0.name.cstr(), hbuf.data(), hbuf.valid_samples(), 1, graph_x_start,{
//            ImPlotProp_LineColor, ImVec(stage0.gr_line),
//            ImPlotProp_Stride, hbuf.stride(),
//            ImPlotProp_Offset , hbuf.slot_of(0),
//        });



        EndPlot();
    }
}
