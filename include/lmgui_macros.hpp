#pragma once 
// MACRO: BUILDER_FN(T, VAR)
// ```cpp
// auto set_##VAR(this auto&& m, T const& var) -> auto&& {
//     m.VAR = var; return std::forward<decltype(m)>(m);
// }
// ```
// Just a shorthand for the builder style setters in spec classes.
// NOTE: if you omit the trailing semicolon, clangd will shit itself and start double indenting everything below,
//       (as if there was unfinished statement)
#define SPEC_BUILDER_SET(T, VAR) auto set_##VAR(this auto&& m, T const& var) -> auto&& { m.VAR = var; return std::forward<decltype(m)>(m);  }
