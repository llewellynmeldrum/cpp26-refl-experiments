#pragma once 
#include "cppslop.hpp"

namespace cfg{
template<typename T> struct Registration{
    Registration() = delete;
    static constexpr auto config_name = "UNDEFINED"sv;
    static constexpr auto description = "UNDEFINED"sv;

};

#define REGISTER_CONFIG(T, NAME, DESC)\
template<>\
struct cfg::Registration<T>{\
    using ConfigType = T;\
    static constexpr auto config_name = CONCAT(NAME,sv); \
    static constexpr auto description = CONCAT(DESC,sv); \
}
} // namespace cfg
