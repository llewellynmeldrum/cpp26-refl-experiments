# cpp26-refl-experiments

Learning C++26 static reflection (P2996, GCC 16.2, `-freflection`) by building a
reflection-driven config system: aggregate struct ⇄ TOML file ⇄ ImGui debug editor.
Target consumer: the user's Minecraft-like project (noise configs, debug options such as
camera fov, spawn position, wireframe, keybinds). Options get added and renamed often.

## Working style
- Discovery mode by default: discuss options and trade-offs, and don't edit, commit or push unless asked.
- KISS: C-like aggregates, explicit data flow, caller-focused API, no speculative abstraction.

## Vocabulary (use consistently)
- When talking about the TOML side, use TOML terms: **table**, **inline table**, **array**,
  **array of tables**, **key**, **value**, **header** (`[a.b]`). Say "array of tables",
  never "array of structs".
- "Array of tables" means an array whose elements are tables. In this project it is always
  written inline (`key = [ {…}, {…} ]`). The TOML spec's `[[key]]` syntax is called the
  **`[[ ]]` syntax**, and we don't emit it.
- Use C++ terms (struct, aggregate, member, `std::vector`) only when talking about the C++ side.

## Settled design decisions
- **The struct is the single source of truth.** Annotations (`Desc`, `Range`, `Rename`, later
  `Inline`/`Compact`) feed all three consumers: TOML write, TOML read and ImGui draw.
- **Default member initializers are allowed.** They fill keys that are missing from the file
  (ImGui `FirstUseEver` semantics) and back a per-field "reset to default" in the UI.
  The file always wins when the key is present.
- **The file is a full dump, regenerated on save.** User comments are erased on purpose.
  Comments are generated metadata: `Desc` on the line(s) above, and a trailing `# type range`.
  Enums list their valid values. Type names come from `stable_type_name`, not `display_string_of`.
- **Custom TOML writer and toml++ reader.** toml++ can't emit comments, which rules out its writer.
  Format values carefully: floats use shortest repr (`std::format("{}")`) and get `.0` only if
  the string has no `.`, `e` or `n` (inf/nan); strings are always basic `"..."` with escapes;
  keys must be bare (`Rename` is checked at compile time).
- **Renames:** no `FormerName`. Unknown keys warn/error on load and are dropped on the next save.
- **Load semantics:** read into a copy of `live` and assign only if the parse succeeds.
  Field-level errors are collected (with toml++ source positions), not fatal.
- **State model:** `T live; T on_disk;`. `dirty = live != on_disk` is derived, not tracked.
  Per-field "modified" highlighting comes from comparing live and on_disk.
- **Saves are explicit only** (Save button). Load button = re-read and apply. Writes are atomic
  (write `.tmp`, then `rename`). If the file is missing at startup, write it from the defaults.
- **API:** low-level `save_toml` / `load_toml` / `draw_fields` plus a convenience
  `config_window(title, ConfigSlot<T>&) -> ConfigEvents{edited, saved, loaded}`.
  The caller owns all state and reacts to events (e.g. regenerate chunks when the noise config
  changes; consider committing edits on `IsItemDeactivatedAfterEdit`).
- **Inline tables are kept.** An inline table is a *value* (written in pass 1), so there's no
  `inline_mode` flag threaded through the block emitter. Use two functions: `emit_inline`
  (a pure recursive expression printer) and the two-pass block emitter (pass 1 = keys with
  values, pass 2 = `[table]` headers). Inline is chosen by a type-level annotation, with an
  optional field-level override, and is forced below any inline table and for every element
  of an array of tables. TOML 1.0 inline tables can't hold comments or newlines, so arrays of
  tables are written one element per line, with a schema comment above and a trailing
  per-element comment.
- **Arrays of tables are always inline; the `[[ ]]` syntax is never emitted.** So an array of
  tables is always a value (pass 1), and only non-inline tables produce headers. A
  `std::vector` in the file replaces the code default wholesale (no element merging).
  Missing keys inside an element default from `Elem{}`. Trade-off accepted: deeply nested
  arrays of tables produce long single lines (multi-line inline tables need TOML 1.1).
- **No `Kind` enum for template dispatch.** Use disjoint concepts (`has_codec`, `toml_scalar`,
  `toml_array`, `toml_struct`, each excluding the earlier ones) plus a `needs_header<T>`
  predicate (`toml_struct<T> && !is_inline<T>`; arrays never need headers). Filter with `if constexpr` inside `template for`, and end each chain with
  `else static_assert(false, ...)`. A `switch` on a constexpr enum does not discard branches.
  An enum only makes sense in a runtime schema-table design.
- **Annotations are gathered once** per field into a plain `FieldMeta` via a consteval
  `field_meta(info)`.
- **Leaf types we don't own** (e.g. `glm::vec3`, which uses anonymous unions) go through a
  `Codec<T>` class template specialization, not overloads (two-phase lookup/ADL pitfall).

## toml++ facts (vendored v3.4.0, checked against the header)
- `toml::table` is a `std::map`, so keys come out sorted and member declaration order is lost.
  That's one reason the writer doesn't build a `toml::table`.
- `node.value<T>()` is lenient: int→float (only if exactly representable), whole float→int,
  **int↔bool**, and range-checked integer narrowing (empty `optional` if out of range).
  `value_exact<T>()` only accepts the native types (`int64_t`, `double`, `bool`, `std::string`, …).
  Use `value_exact<bool>` for bools so `wireframe = 1` is rejected.
- Exceptions are on by default: `parse_file` returns `toml::table` and throws `toml::parse_error`.
  With `TOML_EXCEPTIONS=0` it returns a `parse_result` (`if (!res) res.error()`).
- Source positions: `node.source().begin.line/.column` (1-based), `source().path`, and
  `toml::key::source()` for keys (useful for unknown-key warnings).
- Multi-line inline tables (toml/issues/516) are only parsed with `TOML_ENABLE_UNRELEASED_FEATURES=1`.
- CMake defines `TOML_HEADER_ONLY` (=1), which makes `tomlplusplus_impl.cpp` redundant. For the
  intended STB style, use `TOML_HEADER_ONLY=0` everywhere and keep the one `TOML_IMPLEMENTATION` TU.

## Keybinds (recommended, not final)
- Prefer one field per action (`struct Keybinds { Chord jump{Key::Space}; ... }`) over
  `std::vector<Keybind{key, mods, action, comment}>`: invalid states can't be represented,
  and the description belongs in `SRL_DESC`, not in a data field. Use `std::vector` only for
  open-ended lists (e.g. debug macros: chord → console command).
- `Chord` can be an inline table (`{ key = "F3", mods = ["Ctrl"] }`, no codec needed) or a
  string (`"Ctrl+F3"`, which needs a `Codec<Chord>`). Leaning towards the inline table.
- Open questions: windowing lib (Makefile hints at SDL), own `Key` enum vs `SDL_Scancode` vs
  `ImGuiKey`, scancode vs keycode, mouse buttons and gamepad in `Key`.

## Known issues in the current code (work in progress, does not compile)
- `serialize_scalar_value` (float) produces invalid TOML for `1e+20`, `inf`, `nan`;
  `'...'` literal strings break on quotes and newlines.
- The overlapping predicates (`is_serial_scalar`, `is_nonscalar_array`, partition helpers)
  should be replaced by the disjoint concepts above.
- `serialize -> std::vector<std::byte>` and `SerialFormat{eToml}` are speculative; return `std::string`.
- Per-file `-freflection` means reflection templates must be instantiated in `.refl.cpp`
  files. Prefer a non-template facade (`bool load(Config&, path, errors&)`).
- Unused deps: magic_enum (replace with `enumerators_of`), range-v3, reflect-cpp (could be
  used as a test oracle).

## Things still to verify on GCC 16.2
Annotations on class types and enumerators; `extract<T>` on annotation reflections; float
round-trip through toml++ (`x == load(save(x))` brute-force test); TOML 1.1 / toml++ support
for multi-line inline tables.
