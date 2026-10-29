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
#define SPEC_SETTER(T, VAR) auto set_##VAR(this auto&& m, T const& var) -> auto&& { m.VAR = var; return std::forward<decltype(m)>(m);  }

#define SPEC_SETTER_NAMED(VERB, T, VAR) auto VERB(this auto&& m, T const& var) -> auto&& { m.VAR = var; return std::forward<decltype(m)>(m);  }


// for boolean types
#define SPEC_SETTER_ENABLE(VAR) auto enable_##VAR(this auto&& m) -> auto&& { m.VAR = true; return std::forward<decltype(m)>(m);  }
// for boolean types where you want a custom name, like lock_position
#define SPEC_SETTER_ENABLE_NAMED(VERB, VAR) auto VERB(this auto&& m) -> auto&& { m.VAR = true; return std::forward<decltype(m)>(m);  }

#define SPEC_ENABLE_FLAG(VERB, FLAG_NAME) auto VERB(this auto&& m) -> auto&& { m.flags |= FLAG_NAME; return std::forward<decltype(m)>(m);  }
