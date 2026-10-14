#pragma once 
#include "primitive_types.hpp"

namespace ConfigMeta{
struct Range{ f64 min, max; };          // appears in a comment and also used for validation of input
struct Description{ char const* txt; }; // comment appears on the same the line
} //namespace ConfigMeta{
#ifdef CLANGD_PARSE
    #define RANGE(min,max) 
    #define DESC(txt) 
#else
    #define RANGE(min, max) [[=ConfigMeta::Range{min,max}]]
    #define DESC(txt)       [[=ConfigMeta::Description{txt}]]
#endif
