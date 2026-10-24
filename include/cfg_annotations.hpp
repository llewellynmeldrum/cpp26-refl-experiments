#pragma once 
#include "common.hpp"
#include "primitive_types.hpp"

#if CLANGD_PARSE
    #define CFG_RANGE(min,max) 
    #define CFG_NOTE(txt) 
    #define CFG_SKIP  
    #define CFG_REGISTER_ROOT_CONFIG(name,desc)    
#else
    #define CFG_RANGE(min,max)                        [[=cfg::Range{min,max}]]
    #define CFG_NOTE(txt)                             [[=cfg::Note{std::define_static_string(txt)}]]
    #define CFG_SKIP                                  [[=cfg::Skip{}]]
    #define CFG_REGISTER_ROOT_CONFIG(name, desc)      [[=cfg::Registration{std::define_static_string(name), std::define_static_string(desc)}]]
#endif
namespace cfg{
struct Registration{
    char const* config_name{"UNDEFINED CONFIG NAME"}; 
    char const* description{"UNDEFINED CONFIG DESCRIPTION"}; 
};

struct Range{
    // NOTE: Maybe Range for string should represent the min and max length of the string
    // TODO: as a rule, scalar elements of containers should inherit the range of their container
    // Also i think that scalar members of a struct with a range should get them too 
    f64 min{-std::numeric_limits<f64>::infinity()};
    f64 max{+std::numeric_limits<f64>::infinity()}; 
    template<typename T>
    static constexpr auto default_value()->Range{
        return {
            .min = static_cast<f64>(std::numeric_limits<T>::lowest() / 2.0),
            .max = static_cast<f64>(std::numeric_limits<T>::max() / 2.0),
        };
    }
};
struct Note{ char const* txt{}; };
struct Skip{};

}; // namespace cfg

