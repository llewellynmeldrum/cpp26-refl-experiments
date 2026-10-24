#pragma once 
#include "common.hpp"
#if CPP26
#include "fixed_string.hpp"
#include <meta>
template<typename T, fixed_string msg>
consteval inline auto static_type_error() -> void{
    static_assert(
        false, 
        std::string("\n\e[31;1m<------------------------------------------------------>\e[0m") +
        "\n\e[31;1m -> TYPE ERROR: \e[35m[with "+
        std::string("T = ") + 
        std::string(display_string_of(^^T)) +
        "]\e[0m : " +
        std::string(msg) +
        " \n\n\n\n\n\n\n\n"
    );
}
#endif // CPP26
