#pragma once 

/*
 There are different types of metrics i would like to consider:
 1. durations -> how long did X take?
 2. periodic counters -> how many times did X happen this frame?
 3. totality counters ->  how many X's are there in total?


*/
#include <span>
#include <type_traits>
#include <vector>
#include <implot.h>
#include "concept_helpers.hpp"
#include "timer.hpp"

namespace MTR{
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
template<typename Rb>
    requires (Rb::capacity >= 1'000)
auto p99(Rb const& m) -> Rb::DataType{
    return nth_percentile_avg(m.span(), .99f);
}

template<typename Rb>
    requires (Rb::capacity >= 10'000) // else the result is pretty meaningless.
auto p99_9(Rb const& m) -> Rb::DataType{
    return nth_percentile_avg(m.span(), .99'9f);
}

template<typename Rb>
auto max(Rb const& m) -> Rb::DataType{
    return std::ranges::max_element(m.span());
}

template<typename Rb>
auto min(Rb const& m) -> Rb::DataType{
    return std::ranges::min_element(m.span());
}
template<typename Rb>
auto mean(Rb const& m) -> Rb::DataType{
    using DataType = typename Rb::DataType;
    auto sum = DataType{};
    for (DataType const& val: m.span()){
        sum+=val;
    }
    return sum / m.count();
}


// SECTION: RINGBUFFER
template <typename tDataType, std::size_t tCapacity = 1'000uz> 
    requires std::is_default_constructible_v<tDataType>
struct RingBuffer{
    using DataType = tDataType;
    static constexpr std::size_t capacity = tCapacity;
    static constexpr bool data_is_floating_point = std::floating_point<DataType>;


    std::vector<DataType> buf = std::vector<DataType>(capacity, DataType{});
    std::size_t write_head = 0uz;
    std::size_t write_count = 0uz;

    auto oldest(this auto& m) -> decltype(auto){
        if (m.write_count >= capacity){
            // the next one to be written has by definition waited the longest.
            return m.buf.at(m.write_head);
        }else{
            // the first one to be written before wraparound occurs is idx=0
            return m.buf.at(0);
        }
    }
    auto youngest(this auto& m) -> decltype(auto){
        return m.buf.at( (m.write_head + capacity -1) % capacity);
    }
    auto offset() const {
        if (write_count < capacity){
            return 0uz;
        }else {
            return write_head;
        }
    }
    constexpr auto stride() const -> std::size_t{
        return sizeof(tDataType);
    }
    auto push(this RingBuffer& m, DataType const& v) -> void{
        m.buf.at(m.write_head) = v;
        m.write_head = (m.write_head+1) % capacity;
        m.write_count++;
    }

    auto data(this auto& m) -> decltype(auto){
        return m.buf.data();
    }
    auto begin(this auto& m) -> auto{
        return m.buf.begin();
    }
    // NOTE: returns the actual WRITTEN end, not the true 'end'
    auto end(this auto& m) -> auto{
        return m.buf.begin() + m.write_head;
    }
    auto span(this auto& m) -> auto{
        return std::span(m.begin(), m.end());
    }
    auto count(this auto& m) -> auto{
        return m.write_count > capacity ? capacity :m.write_count;
    }

};

struct DurationTracker{
    using T = f64;
    using Period = std::milli;
    static constexpr std::size_t Capacity = 1'000;

    RingBuffer<T,Capacity> rb{};
    RingBuffer<T,Capacity> seconds_at{};

    auto x_min() const {
        return seconds_at.oldest();
    }
    auto x_max() const {
        return seconds_at.youngest();
    }

    timer::duration cur_duration;

    std::function<void(void*)> update_callback = [](void* user_data){
        auto& self = *static_cast<DurationTracker*>(user_data);
        self.rb.push(timer::to_milliseconds(self.cur_duration));
        self.seconds_at.push(timer::to_seconds(timer::since_epoch()));
    };
    auto sample_scope_duration() -> timer::ScopeStopwatch{
        return {cur_duration, update_callback, static_cast<void*>(this)};
    }

    auto count() const{
        ASSERT(rb.count() == seconds_at.count());
        return rb.count();
    }
    auto get_x_ptr() const {
        return seconds_at.data();
    }
    auto get_y_ptr() const {
        return rb.data();
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
