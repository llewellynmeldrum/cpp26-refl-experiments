#pragma once 

/*
 There are different types of metrics i would like to consider:
 1. durations -> how long did X take?
 2. periodic counters -> how many times did X happen this frame?
 3. totality counters ->  how many X's are there in total?


*/
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
    auto idx = std::clamp<size_t>(
        std::ceil(p*v.size()), 1, v.size()
    ) - 1;
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
        buf[offset()] = v;
        write_count++;
    }
    // oldest first order
    // eg at(0) = the 1st oldest value, at(3) = the 4th oldest value
    constexpr auto at(this auto& m, u64 idx) -> decltype(auto){
        auto const slot = (m.oldest_slot() + idx) % m.Capacity;
        return m.buf[slot];
    }
    constexpr auto oldest(this auto& m) -> decltype(auto){
        return m.at(m.oldest_slot());
    }
    constexpr auto newest(this auto& m) -> decltype(auto){
        return m.at(m.newest_slot());
    }

    constexpr auto data(this auto& m) -> decltype(auto){
        return m.buf.data();
    }

    constexpr auto oldest_slot(){
        return full() ? write_count % Capacity : 0z;
    }
    constexpr auto newest_slot(){
        ASSERT(!empty());
        return (write_count-1) % Capacity;
    }



    constexpr auto offset() const -> u64{
        return write_count % Capacity;
    }
    constexpr auto empty() const -> bool{
        return write_count == 0 ;
    }

    constexpr auto full() const -> bool{
        return write_count >= Capacity ;
    }
    constexpr auto stride() const -> u64{
        return sizeof(DataType);
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
