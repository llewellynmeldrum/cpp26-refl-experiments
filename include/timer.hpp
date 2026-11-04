#pragma once
#include <print>
#ifndef __cplusplus
	#error "This header is intended for c++ only."
#endif // __cplusplus
#include <libassert/assert.hpp>
#include <chrono>

#include "primitive_types.hpp"
namespace timer{
using impl_clock = std::chrono::steady_clock;
using duration = impl_clock::duration;
using time_point = impl_clock::time_point;

template<typename T>
using nano_duration = std::chrono::duration<T, std::nano>;

using Repr_t = f64;



using s_period = std::ratio<1>;
using millis_period = std::milli;
using nanos_period = std::nano;
using micros_period = std::micro;

[[nodiscard]]
inline time_point now(){
    return impl_clock::now();
}

inline time_point prog_epoch_time;
inline bool has_set_epoch{false};
inline void set_prog_epoch(){
    ASSERT(!has_set_epoch);
    has_set_epoch = true;
    prog_epoch_time = timer::now();
}

[[nodiscard]]
inline duration since(timer::time_point t0){
    return timer::now() - t0;
}
[[nodiscard]]
inline duration since_epoch(){
    return since(prog_epoch_time);
}


template<typename Period>
[[nodiscard]]
constexpr duration to_duration(Repr_t dur){
    return std::chrono::duration_cast<duration>(std::chrono::duration<Repr_t,Period>{dur});
}

template<typename Period>
[[nodiscard]]
constexpr Repr_t from_duration(duration dur){
    return std::chrono::duration<Repr_t, Period>(dur).count();
}

[[nodiscard]]
constexpr duration milliseconds(f64 f){
    return to_duration<millis_period>(f);
}

[[nodiscard]]
constexpr duration seconds(f64 f){
    return to_duration<s_period>(f);
}

[[nodiscard]]
inline Repr_t get_seconds(duration dur){
    return from_duration<s_period>(dur);
}
[[nodiscard]]
inline Repr_t get_nanoseconds(duration dur){
    return from_duration<nanos_period>(dur);
}
[[nodiscard]]
inline Repr_t get_microseconds(duration dur){
    return from_duration<micros_period>(dur);
}
[[nodiscard]]
inline Repr_t get_milliseconds(duration dur){
    return from_duration<millis_period>(dur);
}
[[nodiscard]]
inline Repr_t to_milliseconds(duration dur){
    return from_duration<millis_period>(dur);
}
[[nodiscard]]
inline Repr_t to_seconds(duration dur){
    return from_duration<s_period>(dur);
}

// Sets the value of a duration, with an optional callback function
struct [[nodiscard]] ScopeStopwatch{
    using StoredType = timer::duration;
    StoredType & out;
    std::function<void(void*)> dtor_callback = [](void*){};
    void* user_data = nullptr;
    timer::time_point t0 = timer::now();
    ~ScopeStopwatch(){
        out = timer::now() - t0;
        std::invoke(dtor_callback,user_data);
//        std::println("Scope stopwatch dtor, duration: {}",out);
    } 
};
struct [[nodiscard]] ScopeStopwatchAccumulator{
    using StoredType = timer::duration;
    f64& accumulator;
    StoredType & out;
    std::function<void(void*)> dtor_callback = [](void*){};
    void* user_data = nullptr;
    timer::time_point t0 = timer::now();
    ~ScopeStopwatchAccumulator(){
        out = timer::now() - t0;
        std::invoke(dtor_callback,user_data);
//        std::println("Scope stopwatch dtor, duration: {}",out);
    } 
};
/* example: 
f64 t_ms{};
{
    auto _ = timer::ScopeStopwatch{t_ms,timer::millis_period}; 
    // work
}
// t_ms = duration of work in milliseconds (+ overhead)
// 
//
*/

}
