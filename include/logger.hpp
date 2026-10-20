#pragma once 
#include "style.hpp"
#include "cppslop.hpp"
#include "timer.hpp"

#include <print>
#include <cpptrace/basic.hpp>
#include <cpptrace/utils.hpp>
#include <cstdlib>
#include <libassert/assert.hpp>
static inline thread_local bool show_time_in_logs = false;
inline constexpr void LOG_STR(
    std::string const& title_style, 
    std::string_view title="", 
    std::string const& body_style="", 
    std::string const& body="",
    bool bare=false
){
    if (bare){
        std::println(
            "{}{}{}{}",
            style::reset(),
            body_style,
            body,
            style::reset()
        );
    }else{
        std::println(
            "{}{}{}: {}{}{}",
            title_style,
            title == "" ? "" : std::format("{}:",title),
            show_time_in_logs ? "" : std::format("[{:4.2f}]",timer::get_seconds(timer::since_epoch())),
            style::reset(),
            body_style,
            body,
            style::reset()
        );
    }
}
template <class... _Args>
inline constexpr void LOG_EXIT(int code) {
    auto sty = style::fg_green();
    if (code != EXIT_SUCCESS){
        sty = style::fg_red() + style::underline();
    }
    LOG_STR(
        sty,
        "EXIT  ",
        sty,
        std::format("Exiting with error code {}",code)
    );
    if (code!= EXIT_SUCCESS){
        cpptrace::generate_trace(1).print(); 
    }
    std::exit(code);
}

template <class... _Args>
inline constexpr void LOG_ERROR(std::format_string<_Args...> a_fmt, _Args&&... a_args) {
    LOG_STR(
        style::fg_br_red(),
        "ERROR "sv,
        style::fg_red(),
        std::format(a_fmt,std::forward<_Args>(a_args)...)
    );
}
template <class... _Args>
inline constexpr void LOG_CUSTOM_ERR(std::string_view title, std::format_string<_Args...> a_fmt, _Args&&... a_args) {
    LOG_STR(
        style::fg_br_red(),
        title,
        style::fg_red(),
        std::format(a_fmt,std::forward<_Args>(a_args)...)
    );
}
template <class... _Args>
inline constexpr void LOG_PARSE_ERR(std::format_string<_Args...> a_fmt, _Args&&... a_args) {
    LOG_CUSTOM_ERR("PARSE ERROR",a_fmt,std::forward<_Args>(a_args)...);
}
template <class... _Args>
inline constexpr void LOG_SUB_ERROR(std::format_string<_Args...> a_fmt, _Args&&... a_args) {
    LOG_STR(
        style::fg_br_red(),
        ""sv,
        style::fg_red(),
        std::format(a_fmt,std::forward<_Args>(a_args)...),
        true 
    );
}
template <class... _Args>
inline constexpr void LOG_FATAL(std::format_string<_Args...> a_fmt, _Args&&... a_args) {
    LOG_STR(
        style::fg_br_red(),
        "FATAL "sv,
        style::fg_red(),
        std::format(a_fmt,std::forward<_Args>(a_args)...)
    );
    LOG_EXIT(EXIT_FAILURE);
}
template <class... _Args>
inline constexpr void LOG_INFO(std::format_string<_Args...> a_fmt, _Args&&... a_args) {
    LOG_STR(
        style::fg_blue(),
        "INFO  "sv,
        style::fg_br_blue(),
        std::format(a_fmt,std::forward<_Args>(a_args)...)
    );
}
template <class... _Args>
inline constexpr void LOG_WARN(std::format_string<_Args...> a_fmt, _Args&&... a_args) {
    LOG_STR(
        style::fg_yellow(),
        "WARN  "sv,
        style::fg_br_yellow(),
        std::format(a_fmt,std::forward<_Args>(a_args)...)
    );
}
constexpr auto string_bytes(std::string s){
    auto res = std::string{};
    bool first = true;
    for (const auto ch: s){
        if (!first) res+= " ";
        res += std::format("{:02X}",ch);
        first = false;
    }
    return res;
}
#define LOG_DBG_EXPR(expr) LOG_DBG("{} = {}", #expr, expr);
#define LOG_STR_BYTES(expr) LOG_DBG("{} = {} ({})", #expr, expr, string_bytes(expr));
template <class... _Args>
inline constexpr void LOG_DBG(std::format_string<_Args...> a_fmt, _Args&&... a_args) {
    LOG_STR(
        style::fg_rgb(140,0,190),
        "DBG   "sv,
        style::fg_rgb(190,0,255),
        std::format(a_fmt,std::forward<_Args>(a_args)...)
    );
}
