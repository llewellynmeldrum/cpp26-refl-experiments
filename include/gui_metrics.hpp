#pragma once 

#include <array>
#include <map>

#include <implot.h>
#include <implot_internal.h>

#include "implot_helpers.hpp"

#include "common.hpp"
#include "lmgui_common.hpp"
#include "utils.hpp"
#include "gui.hpp"
#include "metrics.hpp"

#if CPP26
#include "lmgui_tables.hpp"
#include "enum_to_string.hpp"
#endif //CPP26
//

inline auto sparkline( zstring_view title, TrackerStage tracker) ->void;

// given some vec2 with x and y between 0 and 1, returns a logical screen space position

inline auto display_frametime_graph(
    TrackerStage frametime_tracker,
    DrawContext const& dctx,
    bool verbose = false
) -> void{
    using namespace ImPlot;
    auto& hbuf = frametime_tracker.tracker.hbuf;
    auto const g_mean = hbuf.mean(); 
    auto const g_mean_fps = (1000.0 / g_mean);
    auto const g_sd = hbuf.std_dev();
    auto const g_p95 = hbuf.p95();
    auto const g_p99 = hbuf.p99();
    auto const g_max = hbuf.max();
    auto plot_title = std::format(
        "mean: {:3.2f}ms"
        " ({:4.1f} fps)"
        " sd: {:3.2f}"
        " p95: {:3.2f}"
        " p99: {:3.2f}"
        " max: {:3.2f}"
        "###frametime_graph"
        ,
        g_mean, g_mean_fps,
        g_sd,
        g_p95,
        g_p99,
        g_max
    );
    auto hoz_line = [](std::string label, f64 y, glm::vec4 color){
        auto vals = std::array{y};
        PlotInfLines("##", vals.data(), 1, {
            ImPlotProp_Flags, ImPlotInfLinesFlags_Horizontal,
            ImPlotProp_LineColor, ImVec(color)
        });
        TagY(y, color, "%s", label.c_str());
    };
    auto plot_size = !verbose ? glm::vec2{0.45,0.15} : glm::vec2{0.45, 0.2};
    if (verbose){
        ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, dctx.uv2logical_extent({0.01,0.01}));
    }

    ImPlotFlags plot_flags =ImPlotFlags_NoMouseText| ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;
    if (verbose){
        plot_flags = ImPlotFlags_NoMouseText;
    }
    if (BeginPlot(plot_title.c_str(), dctx.uv2logical_extent(plot_size), plot_flags)){
        SetupAxis(ImAxis_X1, "frames", ImPlotAxisFlags_NoLabel);
        // the max X is the X coordinate of the newest sample
        auto max_x = f64(hbuf.total_samples())-1.0;
        // the min X is the X coordinate of the oldest sample
        auto min_x = f64(hbuf.total_samples()) - f64(hbuf.Capacity);
        SetupAxisLimits(ImAxis_X1, min_x, max_x, ImPlotCond_Always);
        SetupAxisFormat(ImAxis_X1, "");


        SetupAxis(ImAxis_Y1, "ms", ImPlotAxisFlags_LockMin);
        SetupAxisLimits(ImAxis_Y1, 0.0, 50.0,ImPlotCond_Once);
        SetupAxisZoomConstraints(ImAxis_Y1, 0.1, 1000.0);
        SetupAxisLimitsConstraints(ImAxis_Y1, 0.0,1000.0);

        SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);

        auto graph_x_start = f64(hbuf.total_samples()) - f64(hbuf.valid_samples());

        PlotLine(frametime_tracker.name.cstr(), hbuf.data(), hbuf.valid_samples(), 1, graph_x_start,{
            ImPlotProp_LineColor, ImVec(frametime_tracker.gr_line()),
            ImPlotProp_Stride, hbuf.stride(),
            ImPlotProp_Offset , hbuf.slot_of(0),
        });
        if (verbose){
            hoz_line(std::format("avg {:3.1f}", g_mean), g_mean, util::_01rgb( 57, 201,  24));
            if (g_p95 > g_mean * 1.35){
                hoz_line(std::format("p95 {:3.1f}", g_p95), g_p95,   util::_01rgb(255, 191, 0));
            }
            if (g_p99 > g_mean * 1.35){
                hoz_line(std::format("p99 {:3.1f}", g_p99), g_p99,   util::_01rgb(215, 100, 10));
            }
            if (g_max > g_mean * 1.35){
                hoz_line(std::format("max {:3.1f}", g_max), g_max,   util::_01rgb(215,  57,  32));
            }
        }

        EndPlot();
    }
    if (verbose){
        ImPlot::PopStyleVar(1);
    }
}

inline auto display_stacked_frametime_breakdown_graph(
    zstring_view title,
    std::vector<TrackerStage> stages,
    DrawContext const& dctx,
    bool verbose = false
) -> void{
    using namespace ImPlot;
    auto plot_size = !verbose ? glm::vec2{0.45,0.15} : glm::vec2{0.45, 0.2};
    auto plot_flags = ImPlotFlags_NoMouseText | ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;
    if (verbose){
        plot_flags = ImPlotFlags_NoMouseText;
    }
    if (BeginPlot(title.cstr(), dctx.uv2logical_extent(plot_size), plot_flags)){
        u64 oldest_shared_sample = 0uz; // oldest shared sample between all stages
        u64 newest_shared_sample = std::numeric_limits<u64>::max(); // newest shared sample between all stages
        for (auto& stage: stages){
            auto const& hbuf = stage.tracker.hbuf;
            oldest_shared_sample = std::max(oldest_shared_sample, hbuf.oldest_sample());
            newest_shared_sample = std::min(newest_shared_sample, hbuf.total_samples());
        }
        auto num_samples = newest_shared_sample > oldest_shared_sample ? 
            newest_shared_sample - oldest_shared_sample : 0;
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
        SetupAxis(ImAxis_X1, "frames", ImPlotAxisFlags_NoLabel);
        // the max X is the X coordinate of the newest sample
        auto max_x = f64(newest_shared_sample) - 1.0;
        auto window = f64(stages[0].tracker.hbuf.Capacity) - 1.0;
        SetupAxisLimits(ImAxis_X1, max_x-window, max_x, ImPlotCond_Always);
        SetupAxisFormat(ImAxis_X1, "");

        SetupAxis(ImAxis_Y1, "ms", ImPlotAxisFlags_LockMin);
        SetupAxis(ImAxis_Y1, "ms", ImPlotAxisFlags_LockMin);
        SetupAxisLimits(ImAxis_Y1, 0.0, 50.0,ImPlotCond_Once);
        SetupAxisZoomConstraints(ImAxis_Y1, 0.1, 1000.0);
        SetupAxisLimitsConstraints(ImAxis_Y1, 0.0,1000.0);


        SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);
        SetupFinish();
        for (auto const& [stage_idx, stage] : std::views::enumerate(stages)){
            if (!stage.check_enabled()){
                // skip stages which are hidden by the legend
                continue;
            }
            auto const& hbuf = stage.tracker.hbuf;
            for (auto idx = 0uz; idx < num_samples; idx++){ // might need to be idx<num_samples-1
                auto& y0 = stage_y_start.at(stage_idx);
                auto& y1 = stage_y_stop.at(stage_idx);

                auto stage_sample = hbuf.sample(oldest_shared_sample + idx);

                y0.at(idx) = y_offset_cumulative.at(idx);
                y1.at(idx) = y_offset_cumulative.at(idx) + stage_sample;
                y_offset_cumulative.at(idx) += stage_sample; 
            }
        }

        for (auto const& [stage_idx, stage] : std::views::enumerate(stages)){
            auto const& hbuf = stage.tracker.hbuf;
            auto const& y_lo = stage_y_start[stage_idx];
            auto const& y_hi = stage_y_stop[stage_idx];
            ASSERT(xs.size() == y_lo.size() && y_lo.size() == y_hi.size());
            auto const N = xs.size();
            PlotShaded(stage.name.cstr(), xs.data(), y_lo.data(), y_hi.data(), N,{
                ImPlotProp_FillColor, ImVec(stage.gr_fill),
                ImPlotProp_FillAlpha, 0.5f,
                ImPlotProp_Stride, hbuf.stride(),
            });
        }
        EndPlot();

        #if CPP26
        if (verbose){
            auto spec = ui::TableSpec{}
                .set_title(title.clone())
                .set_flags(ui::TableFlagBits::eRowBg)
                .set_column_headers({
                    ui::TableColumnHeader{"subsystem",  ui::TableColumnFlagBits::eWidthFixed},
                    ui::TableColumnHeader{"mean",       ui::TableColumnFlagBits::eWidthFixed},
                    ui::TableColumnHeader{"sd",         ui::TableColumnFlagBits::eWidthFixed},
                    ui::TableColumnHeader{"p95",        ui::TableColumnFlagBits::eWidthFixed},
                    ui::TableColumnHeader{"p99",        ui::TableColumnFlagBits::eWidthFixed},
                    ui::TableColumnHeader{"max",        ui::TableColumnFlagBits::eWidthFixed},
                    ui::TableColumnHeader{"graph",      ui::TableColumnFlagBits::eWidthStretch},
                })
                .set_cell_padding(glm::vec2(5.0f, 5.0f))
            ;
            if (auto table = ui::begin_scope(spec)){
                for (const auto& stage: std::views::reverse(stages)){
                    if (!stage.was_enabled_this_frame) continue;
                    table.next_row()
                        .column([&]{ ui::Label(stage.name.cstr()); })
                        .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.std_dev())); })
                        .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.max())); })
                        .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.p95())); })
                        .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.p99())); })
                        .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.max())); })
                        .column([&]{ sparkline(stage.name.cstr(), stage);})
                    ;
                }
            }
        }
        #endif
    }
}

inline auto sparkline(
    zstring_view title,
    TrackerStage tracker
) ->void{
    using namespace ImPlot;
    auto plot_size = glm::vec2{ui::AvaliableRegion().x, ImGui::GetFrameHeight()};

    auto const plot_flags = ImPlotFlags_CanvasOnly | ImPlotFlags_NoInputs;
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, glm::vec2{0,0});
    if (BeginPlot(title.cstr(), plot_size, plot_flags)){
        SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoDecorations);
        SetupAxis(ImAxis_Y1, nullptr, ImPlotAxisFlags_NoDecorations | ImPlotAxisFlags_AutoFit);
        auto& hbuf = tracker.tracker.hbuf;
        // the max X is the X coordinate of the newest sample
        auto max_x = f64(hbuf.total_samples())-1.0;
        // the min X is the X coordinate of the oldest sample
        auto min_x = f64(hbuf.total_samples()) - f64(hbuf.Capacity);
        SetupAxisLimits(ImAxis_X1, min_x, max_x, ImPlotCond_Always);
        SetupAxisFormat(ImAxis_X1, "");

        auto graph_x_start = f64(hbuf.total_samples()) - f64(hbuf.valid_samples());

        PlotLine(tracker.name.cstr(), hbuf.data(), hbuf.valid_samples(), 1, graph_x_start,{
            ImPlotProp_LineColor, ImVec(tracker.gr_line()),
            ImPlotProp_Stride, hbuf.stride(),
            ImPlotProp_Offset , hbuf.slot_of(0),
        });
        PlotShaded(tracker.name.cstr(), hbuf.data(), hbuf.valid_samples(), 0, 1, graph_x_start, {
            ImPlotProp_FillColor, ImVec(tracker.gr_fill),
            ImPlotProp_FillAlpha, 0.5f,
            ImPlotProp_Stride, hbuf.stride(),
            ImPlotProp_Offset , hbuf.slot_of(0),
        });

        EndPlot();
    }
    ImPlot::PopStyleVar(1);
}
