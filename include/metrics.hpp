#pragma once 

/*
 There are different types of metrics i would like to consider:
 1. durations -> how long did X take?
 2. periodic counters -> how many times did X happen this frame?
 3. totality counters ->  how many X's are there in total?


*/
#include <limits>
#include <ranges>
#include <span>
#include <type_traits>
#include <vector>
#include <functional>
#include <implot.h>
#include "concept_helpers.hpp"
#include "cppslop.hpp"
#include "timer.hpp"

namespace MTR{
struct with_accumulator_t{};
inline constexpr auto with_accumulator = with_accumulator_t{};

// SECTION: GENERIC MATH UTILS
template<typename T> requires is_numeric<T>
auto nth_percentile_avg(std::span<const T> a_span, f64 p) -> T{
    ASSERT(0.0<p && p<1.0); // 0.99 == 99th percentile
    auto v = std::vector(std::from_range, a_span);
    if (v.size() == 0){
        return T{0};
    }else if (v.size() == 1){
        return a_span[0];
    }
    auto rank = st_cast<size_t>(
        std::ceil(p * f64(v.size()))
    );
    auto idx = std::clamp<size_t>(rank, 1, v.size()) - 1;
    auto nth = v.begin() + static_cast<std::ptrdiff_t>(idx);
    std::ranges::nth_element(v, nth);
    return v[idx];

}

// SECTION: RINGBUFFER MATH UTILS 

// SECTION: RINGBUFFER
template <typename T, i64 tCapacity = 1'000z> 
    requires std::is_default_constructible_v<T>
struct HistoryBuffer{
    using DataType = T;
    static constexpr u64 Capacity = tCapacity;


    u64 write_count = 0uz;
    std::vector<DataType> buf = std::vector(Capacity, DataType{});

    constexpr auto push(DataType const& v) -> void{
        buf[write_slot()] = v;
        write_count++;
    }
    // oldest first order
    // eg at(0) = the 1st oldest value, at(3) = the 4th oldest value
    constexpr auto at(this auto& m, u64 idx) -> decltype(auto){
        auto const slot = (m.oldest_slot() + idx) % m.Capacity;
        return m.buf[slot];
    }
    constexpr auto oldest(this auto& m) -> decltype(auto){ return m.buf[m.oldest_slot()]; }
    constexpr auto newest(this auto& m) -> decltype(auto){ return m.buf[m.newest_slot()]; }

    constexpr auto data(this auto& m) -> decltype(auto){
        return m.buf.data();
    }

    // time index -> slot, i.e 0 is the oldest, 1 is the 2nd oldest, 2 is the third oldest
    // returns the ith oldest slot
    constexpr auto slot_of(u64 i) const -> u64{
        auto base_slot = full() ? write_slot() : 0uz;
        return (base_slot + i) % Capacity;
    }
    constexpr auto oldest_slot() const -> u64{ ASSERT(!empty()); return slot_of(0);}
    constexpr auto newest_slot() const -> u64{ ASSERT(!empty()); return slot_of(valid_samples()-1);}



    constexpr auto write_slot() const -> u64{ return write_count % Capacity; }

    // Self explanatory
    constexpr auto empty() const -> bool{ return write_count == 0 ; }

    // Have the slots been filled, and overwrites have begun?
    constexpr auto full() const -> bool{ return write_count >= Capacity ; }

    // valid elements count.
    constexpr auto stride() const -> u64{ return sizeof(DataType); }
    constexpr auto total_samples() const -> u64{ return write_count; }
    constexpr auto valid_samples() const -> u64{ return full() ? Capacity : write_count; }

    // aka x_offset in the graph
    constexpr auto oldest_sample() const -> u64 {return total_samples() - valid_samples(); }

    // if the sample is at least the oldest, and is not the write_count'th, which is the next to be written
    constexpr auto contains_sample(u64 s) const -> bool {
        return oldest_sample() <= s && s < write_count;
    }
    constexpr auto sample(this auto& m, u64 s) -> decltype(auto){
        ASSERT(m.contains_sample(s));
        return m.buf[s % Capacity];
    }


    constexpr auto mean() const -> T{
        auto const n = valid_samples();
        auto sum = T{0};
        for (auto i = 0uz; i<n; i++){
            sum+=buf[i];
        }
        return sum/n;
    }
    constexpr auto max() const -> T{
        auto const n = valid_samples();
        auto res = std::numeric_limits<T>::lowest();
        for (auto i = 0uz; i<n; i++){
            res = std::max(res,buf[i]);
        }
        return res;
    }
    constexpr auto std_dev() const -> T{
        auto const N = valid_samples();
        auto const avg = mean();
        auto squared_differences = T{0};

        for (auto i = 0uz; i<N; i++){
            squared_differences += std::pow(buf[i] - avg,2);
        }

        auto const variance = squared_differences / N ;
        return std::sqrt(variance);
    }
    constexpr auto p95() const -> T{
        return nth_percentile_avg(std::span(buf.begin(), buf.begin() + valid_samples()), 0.95);
    }
    constexpr auto p99() const -> T{
        return nth_percentile_avg(std::span(buf.begin(), buf.begin() + valid_samples()), 0.99);
    }
};



// Wraps a HistoryBuffer and provides a scoped stopwatch adder
struct DurationTracker{
    using T = f64;
    using Period = std::milli;
    static constexpr std::size_t Capacity = 1'000;


    HistoryBuffer<T,Capacity> hbuf{};
    timer::duration cur_duration;

    std::function<void(void*)> update_callback = [](void* user_data){
        auto& self = *static_cast<DurationTracker*>(user_data);
        auto ms = timer::to_milliseconds(self.cur_duration);
        self.hbuf.push(ms);
    };
    auto push_scope_duration() -> timer::ScopeStopwatch{
        return {cur_duration, update_callback, static_cast<void*>(this)};
    }

};
/*
 So ideally:
 DurationTracker ms_frametime;


 while(is_running){
    auto _ = ms_frametime.measure_scope();
    // do normal work.
    // ...
    // ...
 } // dtor of the scope object runs, which locks in the measurement. 
*/


};
