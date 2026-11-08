#pragma once 

#include <array>
#include <imgui_internal.h>
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


// given some vec2 with x and y between 0 and 1, returns a logical screen space position

struct HistoryBufferStats{
    using T = f64;
    HistoryBufferStats(MTR::HistoryBuffer<T> const& hbuf)
        : mean(hbuf.mean()) 
        , sd(hbuf.std_dev())
        , p95(hbuf.p95())
        , p99(hbuf.p99())
        , max(hbuf.max())
    {}
    T mean;
    T sd;
    T p95;
    T p99;
    T max;
    auto summary() const noexcept -> std::string{
        return std::format(
            "mean: {:3.2f}ms"
            " ({:4.1f} fps)"
            " sd: {:3.2f}"
            " p95: {:3.2f}"
            " p99: {:3.2f}"
            " max: {:3.2f}"
            "###frametime_graph"
            ,
            mean, 
            (1000.0 / mean),
            sd,
            p95,
            p99,
            max
        );
    }
    auto summary_one_line() const noexcept -> std::string{
        return std::format(
            "mean: {:3.2f}ms"
            "  ({:4.1f} fps)"
            "  sd: {:3.2f}"
            "  p95: {:3.2f}"
            ,
            mean, 
            (1000.0 / mean),
            sd,
            p95
        );
    }
};
inline auto draw_hoz_line (f64 y, glm::vec4 color) -> void{
    auto vals = std::array{y};
    ImPlot::PlotInfLines("##", vals.data(), 1, {
        ImPlotProp_Flags, ImPlotInfLinesFlags_Horizontal,
        ImPlotProp_LineColor, ImVec(color)
    });
};
inline auto draw_frametime_graph(
    HistoryBufferStats const& stats,
    TrackerStage const& frametime_tracker,
    GuiDrawContext const& dctx,
    zstring_view title = "##"
) -> void{
    using namespace ImPlot;
    auto const& verbose = dctx.overlay_mode == OverlayMode::Full;
    auto& hbuf = frametime_tracker.tracker.hbuf;
    auto plot_size = !verbose ? glm::vec2{0.45,0.15} : glm::vec2{0.45, 0.2};
    if (verbose){
        ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, dctx.uv2logical_extent({0.01,0.01}));
        // TODO: make a good floating/transparent mode
    }

    ImPlotFlags plot_flags =ImPlotFlags_NoMouseText| ImPlotFlags_NoLegend | ImPlotFlags_NoTitle;
    if (verbose){
        plot_flags = ImPlotFlags_NoMouseText;
    }
    if (BeginPlot(title.cstr(), dctx.uv2logical_extent(plot_size), plot_flags)){
        auto const x_axis_flags = verbose ? ImPlotAxisFlags_NoLabel : ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
        SetupAxis(ImAxis_X1, "frames", x_axis_flags);
        // the max X is the X coordinate of the newest sample
        auto max_x = f64(hbuf.total_samples())-1.0;
        // the min X is the X coordinate of the oldest sample
        auto min_x = f64(hbuf.total_samples()) - f64(hbuf.Capacity);
        SetupAxisLimits(ImAxis_X1, min_x, max_x, ImPlotCond_Always);
        SetupAxisFormat(ImAxis_X1, "");


        SetupAxis(ImAxis_Y1, "ms", ImPlotAxisFlags_LockMin);
        SetupAxisLimits(ImAxis_Y1, 0.0, 17.0,ImPlotCond_Once);
        SetupAxisZoomConstraints(ImAxis_Y1, 0.1, 1000.0);
        SetupAxisLimitsConstraints(ImAxis_Y1, 0.0,1000.0);

        SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);

        auto graph_x_start = f64(hbuf.total_samples()) - f64(hbuf.valid_samples());

        PlotLine(frametime_tracker.name.cstr(), hbuf.data(), hbuf.valid_samples(), 1, graph_x_start,{
            ImPlotProp_LineColor, ImVec(util::_01rgb(255,255,255,255)),
            ImPlotProp_Stride, hbuf.stride(),
            ImPlotProp_Offset , hbuf.slot_of(0),
        });
        if (verbose){
            auto const col = util::_01rgb(128,128,128,255);
            auto y_60hz = 1000.0 / 60.0;
            Annotation(max_x, y_60hz, col,glm::vec2{0,0},true,"60fps");
            draw_hoz_line(y_60hz, col);

            auto y_144hz = 1000.0 / 144.0;
            Annotation(max_x, y_144hz, col,glm::vec2{0,0},true,"144fps");
            draw_hoz_line(y_144hz, col);
        }

        EndPlot();
    }
    if (verbose){
        ImPlot::PopStyleVar(1);
    }
}

inline auto find_oldest_and_newest_shared_sample(std::span<const TrackerStage> stages) -> std::pair<u64,u64>{
    u64 oldest_shared_sample = 0uz; // oldest shared sample between all stages
    u64 newest_shared_sample = std::numeric_limits<u64>::max(); // newest shared sample between all stages
    for (auto& stage: stages){
        auto const& hbuf = stage.tracker.hbuf;
        oldest_shared_sample = std::max(oldest_shared_sample, hbuf.oldest_sample());
        newest_shared_sample = std::min(newest_shared_sample, hbuf.total_samples());
    }
    return {oldest_shared_sample, newest_shared_sample};
}
inline auto draw_stage_breakdown_table(zstring_view title, std::span<const TrackerStage> stages){
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
                .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.mean())); })
                .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.std_dev())); })
                .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.p95())); })
                .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.p99())); })
                .column([&]{ ui::Label(std::format("{:.2f}",stage.tracker.hbuf.max())); })
//                .column([&]{ sparkline(stage.name.cstr(), stage);})
            ;
        }
    }
}
inline auto calculate_stacked_areas(
    u64 n_samples,
    u64 oldest_shared_sample,
    std::span<TrackerStage> stages
){
    // NOTE: 
    // Since ImPlot doesnt possess the utilities to have a proper, stacked area chart like I want, 
    // we will have to the stacking manually. Here is an outline of how we do that:
    //
    // for each stage, we need to find the y_start (bottom of the filled in area at some point)
    // So therefore stage_y_start[STAGE_0][10] ==> the lower y that STAGE_0's area will fill starting from, for sample 10.
    // and,         stage_y_stop[STAGE_0][10]  ==> the UPPER y that STAGE_0's area will END filling AT, for sample 10.
    //
    // Eg;
    //           _____<-- stage_y_stop[STAGE_1][200] - the UPPER y value of STAGE_1 at sample 200.
    //           | S1|
    //           |   |
    //           |---|<-- stage_y_stop[STAGE_0][200] - the UPPER y value of STAGE_0 at sample 200.
    //           | S0|    It also happens to be stage_y_start[STAGE_1][200].
    //  0....199 |___|
    //             ^
    //             |
    //           stage_y_start[STAGE_0][200]
    //           The lower y value of STAGE_0 at sample 200.
    //
    auto stage_y_start = std::vector(stages.size(), std::vector(n_samples, f64{}));
    auto stage_y_stop = std::vector(stages.size(), std::vector(n_samples, f64{}));
    auto y_offset_cumulative = std::vector(n_samples, f64{}); // Keep track of the cumulative offset. Kind of unnecessary, but makes the math clearer
    for (auto const& [stage_idx, stage] : std::views::enumerate(stages)){
        if (!stage.check_enabled()){
            // skip stages which are hidden by the legend
            continue;
        }
        auto const& hbuf = stage.tracker.hbuf;
        for (auto idx = 0uz; idx < n_samples; idx++){ // might need to be idx<num_samples-1
            auto& y0 = stage_y_start.at(stage_idx);
            auto& y1 = stage_y_stop.at(stage_idx);

            auto stage_sample = hbuf.sample(oldest_shared_sample + idx);

            y0.at(idx) = y_offset_cumulative.at(idx);
            y1.at(idx) = y_offset_cumulative.at(idx) + stage_sample;
            y_offset_cumulative.at(idx) += stage_sample; 
        }
    }
    return std::make_pair(stage_y_start,stage_y_stop);
}
inline auto draw_stacked_frametime_breakdown_graph(
    std::span<TrackerStage> stages,
    GuiDrawContext const& dctx,
    zstring_view title ="##"
) -> void{
    using namespace ImPlot;
    auto const& verbose = dctx.overlay_mode == OverlayMode::Full;
    if (!verbose){
        ImPlot::PushStyleColor(ImPlotCol_FrameBg, util::_01rgb(0,0,0,40));
        ImPlot::PushStyleColor(ImPlotCol_PlotBg, util::_01rgb(0,0,0,70));
    }
    auto const plot_size = dctx.uv2logical_extent(
        !verbose ? glm::vec2{0.45,0.15} : glm::vec2{0.45, 0.2}
    );
    auto const plot_flags =  ImPlotFlags_NoMouseText;
//    char const* title = "Subsystem Frametime Contribution";
    if (BeginPlot(title.cstr(), plot_size, plot_flags)){
        auto [oldest_shared_sample, newest_shared_sample] = find_oldest_and_newest_shared_sample(stages);
        auto const n_samples = 
            newest_shared_sample <= oldest_shared_sample ? 0 
            : newest_shared_sample - oldest_shared_sample;

        // we also need to make the xs, which will be oldest_sample -> newest_sample
        auto xs = std::vector(n_samples, f64{});
        for (auto idx = 0uz; idx < n_samples; idx++){ // might need to be idx<num_samples-1
            xs.at(idx) = oldest_shared_sample + idx;
        }

        
        auto const x_axis_flags = verbose ? ImPlotAxisFlags_NoLabel : ImPlotAxisFlags_NoTickLabels | ImPlotAxisFlags_NoLabel;
        SetupAxis(ImAxis_X1, "frames", x_axis_flags);

        
        auto const max_x = f64(newest_shared_sample) - 1.0;// the max X is the X coordinate of the newest sample

        auto const window_size = f64(stages[0].tracker.hbuf.Capacity) - 1.0;
        auto const min_x = max_x - window_size;// the min X is the X coordinate of the oldest sample
        SetupAxisLimits(ImAxis_X1, min_x, max_x, ImPlotCond_Always);

        SetupAxis(ImAxis_Y1, "ms", ImPlotAxisFlags_LockMin);
        SetupAxisLimits(ImAxis_Y1, 0.0, 17.0,ImPlotCond_Once);
        SetupAxisZoomConstraints(ImAxis_Y1, 0.1, 1000.0);
        SetupAxisLimitsConstraints(ImAxis_Y1, 0.0,1000.0);


        SetupLegend(ImPlotLocation_NorthWest, ImPlotLegendFlags_Horizontal);
        SetupFinish();
        auto const [stage_y_start, stage_y_stop] = calculate_stacked_areas(
            n_samples,
            oldest_shared_sample,
            stages
        );

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
        if (!verbose){
            auto const col = util::_01rgb(128,128,128,255);
            auto y_60hz = 1000.0 / 60.0;
            Annotation(max_x, y_60hz, col,glm::vec2{0,0},true,"60fps");
            draw_hoz_line(y_60hz, col);

            auto y_144hz = 1000.0 / 144.0;
            Annotation(max_x, y_144hz, col,glm::vec2{0,0},true,"144fps");
            draw_hoz_line(y_144hz, col);
        }

        EndPlot();

        #if CPP26
        if (verbose){ draw_stage_breakdown_table(title, stages); }

        #endif
    }
    if (!verbose){
        ImPlot::PopStyleColor(2);
    }
}

struct SparklineSettings{
    zstring_view title;
    f32 ems=1.0f; // 1em = 1 frame height
    std::optional<HistoryBufferStats> stats=std::nullopt;
    bool y_tick_labels = false;
};
inline auto sparkline(
    TrackerStage tracker,
    SparklineSettings s
) ->void{
    using namespace ImPlot;
    auto plot_size = glm::vec2{ui::AvaliableRegion().x, ImGui::GetFrameHeight() * s.ems};

    if (s.stats){
        ui::Label(std::format("{}:{}",s.title,(*s.stats).summary_one_line()));
    }
    auto const plot_flags = ImPlotFlags_CanvasOnly | ImPlotFlags_NoBoxSelect |ImPlotFlags_NoMenus;
    ImPlot::PushStyleVar(ImPlotStyleVar_PlotPadding, glm::vec2{0,0});
    if (BeginPlot(s.title.cstr(), plot_size, plot_flags)){
        auto const y_flags = s.y_tick_labels ? ImPlotAxisFlags_NoLabel : ImPlotAxisFlags_NoDecorations;
        SetupAxis(ImAxis_X1, nullptr, ImPlotAxisFlags_NoDecorations);
        SetupAxis(ImAxis_Y1, nullptr, y_flags);
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
        if (ImPlot::IsPlotHovered()){
            if(ImGui::BeginTooltip()){
                ImGui::TextDisabled("Double click to auto fit.");
                ImGui::EndTooltip();
            }

        }

        EndPlot();
    }
    ImPlot::PopStyleVar(1);
}
