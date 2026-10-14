#pragma once 
#if !defined(__cpp_impl_reflection) && !defined(CLANGD_PARSE)
#error "This header uses C++26 reflection features, ensure this TU is compiled with -freflection."
#endif
#include <meta>
namespace meta = std::meta;
