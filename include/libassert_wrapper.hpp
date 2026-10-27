#pragma once 
#include <libassert/assert.hpp>
 #if LIBASSERT_IS_CLANG 
  #define ASSERT_IF_NOT_CLANG(...) 
 #else
  #define ASSERT_IF_NOT_CLANG(...) LIBASSERT_ASSERT(__VA_ARGS__)
 #endif


