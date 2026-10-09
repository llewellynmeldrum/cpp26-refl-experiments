#include <algorithm>
#include <meta>
#include <string_view>
#include <print>
#include <chrono>
#include <filesystem>
#include <ranges>
#include <iostream>


namespace fs = std::filesystem;
using namespace std::string_view_literals;

#include "primitive_types.hpp"
#include "testing_refl.refl.hpp"
#include "reflect_formatter.refl.hpp"
//#include "meta_helpers.refl.hpp"

#include "serialize_traits.hpp"
#include "serial_format.hpp"

#include "file_io.hpp"
#include "test_types.hpp"


struct Name{
    std::string first = "Tom";
    std::string last = "Preston-Werner";
};
struct Point{
    int x = 1;
    int y = 2;
};
struct Contact{
    std::string name;
    std::string email;
};
struct ContactList{
    Contact personal{
        "Donald Duck",
        "donald@duckburg.com",
    };
    Contact work{
        "Coin cleaner",
        "donald@ScroogeCorp.com",
    };
};
struct TestRootTable{
    Name name;
    Point point;
    ContactList contact;
};

namespace meta = std::meta;

consteval auto is_inline_serializable(std::meta::info m) -> bool{
    if (satisifes_concept(^^is_serializable_scalar, meta::type_of(m))){
        return true;
    }else if(satisifes_concept(^^std::ranges::contiguous_range, meta::type_of(m))){
        auto r_value_type = meta::dealias(
            meta::substitute(^^std::ranges::range_value_t, {meta::type_of(m)})
        );
        return satisifes_concept(^^is_serializable_scalar, r_value_type);
    }else {
        return false;
    }
}
consteval auto is_serial_scalar(std::meta::info m) -> bool{
    return satisifes_concept(^^is_serializable_scalar, meta::type_of(m));
}
consteval auto is_nonscalar_array(std::meta::info m) -> bool{
    return satisifes_concept(^^std::ranges::input_range, meta::type_of(m));
}
consteval auto is_nonscalar_non_array(std::meta::info m) -> bool{
    return !is_serial_scalar(m) && !is_nonscalar_array(m);
}

template<typename T>
consteval auto partition_fields(){
    //
    return std::define_static_array(
        nonstatic_members_partitioned<T>(is_inline_serializable)
    );
}
template<typename T>
consteval auto nonstatic_scalars(){
    return std::define_static_array(
        
    );
}
// If we werent in meta land, i would like to separate out the 
template<typename T>
consteval auto nonstatic_array_like(){
    return std::define_static_array(
        nonstatic_members_if<T>(is_array_like)
    );
}
template<typename T>
consteval auto nonstatic_non_scalars(){
    //
    return std::define_static_array(
        nonstatic_members_if<T>(is_serial_nonscalar)
    );
}
namespace refl{


// NOTE: SERIALIZATION SHIT 
// ----------------------------------------------------------------------------------------------------
//
//
//template<typename T>
struct SerializeContext{
    std::string_view key_identifier;
    std::vector<std::string> nested_headers;
    bool inline_mode;
};
template<typename T>
[[nodiscard]] auto serialize_table(
    T const& value;
    SerializeContext<T> ctx
) -> std::string;

struct TomlConfig{
    constexpr static bool enable_type_name_comments = false;
    constexpr static bool enable_auto_indent = false;
    constexpr static bool enable_newline_after_tables = false;
    constexpr static bool omit_empty_tables = true ; //TODO: implement
};

// We are going to use inline tables for arrays of tables.




consteval auto type_matches(meta::info a, meta::info b){
    return meta::remove_cv(meta::dealias(a)) == meta::remove_cv(meta::dealias(b));
}

template<typename T>
    requires is_serializable_scalar<T>
auto serialize_scalar_no_inline(T const& v, std::string_view key) -> std::string{
    auto val_str = serialize_scalar_value<T>(v);
    auto res = std::string{};
    res += std::format("{} = {}", key, val_str);
    if constexpr (TomlConfig::enable_type_name_comments){
        if constexpr (std::convertible_to<T, bool>){
            res += std::format(" # {}", stable_type_name(^^T));
        }
    }
    res+= "\n";
    return res;
}
template<typename T>
    requires is_serializable_scalar<T>
auto serialize_scalar_inline(T const& v, std::string_view key) -> std::string{
    auto val_str = serialize_scalar_value<T>(v);
    auto res = std::string{};
    res += std::format("{} = {}", key, val_str);
    if constexpr (TomlConfig::enable_type_name_comments){
        if constexpr (std::convertible_to<T, bool>){
            res += std::format(" # {}", stable_type_name(^^T));
        }
    }
    res+= ", ";
    return res;
}

constexpr auto indent_n(size_t indent, std::string indent_chs="  ")->std::string{
    if constexpr(!TomlConfig::enable_auto_indent){
        return "";
    }
    auto s = std::string{};
    for (size_t i = 0; i<indent-1; i++){
        s+=indent_chs;
    }
    return s;
}

auto serialize_header(
    std::vector<std::string> nested_headers,
    bool double_brace = false
){
    auto open_brace =   std::string(double_brace ? "[[" : "[");
    auto close_brace =  std::string(double_brace ? "]]" : "]");
    bool first = true;
    auto res =std::string{};
    res += indent_n(nested_headers.size());
    res += open_brace;
    for (auto const& header : nested_headers){
        if (!first) res+=".";
        res += std::format("{}",header);
        first = false;
    }
    res+= close_brace;
    res+= "\n";
    return res;
}

template<typename value_type>
[[nodiscard]] auto serialize_array(
    std::span<const value_type> array,
    std::string_view identifier,
    std::vector<std::string> nested_headers
) 
-> std::string{
    auto res = std::string{};
    // TODO: support arrays of arrays 
    // This would require checking to see if the non trivially serializable object is actually just another array
    // N times (recursively probably)
    if constexpr (is_serializable_scalar<value_type>){
        // if the value type is trivial, we simply put it in a one line array:
        // IDENTIFIER = [1, 2, 3]
        bool first = true;
        res += indent_n(nested_headers.size());
        res += std::format("{} = ",identifier);
        res+= "[";
        for (auto const& v: array){
            if (!first) res+= ", ";
            res += serialize_scalar_key_value<value_type>();
            first = false;
        }
        res+= "]";
    }else{
        // Otherwise, 
        // 
        // [[IDENTIFIER]]
        // field_one = 'A'
        // field_two = 'B'
        //
        // [[IDENTIFIER]]
        // field_one = 'A'
        // field_two = 2
        auto first = true;
        for (auto const& v: array){
//            if (!first) res+= "\n";
            nested_headers.push_back(std::string(identifier));
            res += serialize_header(nested_headers,true);
            res += serialize_table(v, identifier, nested_headers,true);
            nested_headers.pop_back();
            first = false;
        }

    }
    return res;
}
consteval auto get_identifier(meta::info m) -> std::string_view{
    auto annotations = meta::annotations_of(m);
    auto name = meta::identifier_of(m);
    for (auto const& ann: annotations){
        if (meta::remove_cv(meta::type_of(ann)) == ^^FieldRename){
            name = meta::extract<FieldRename>(ann).new_name;
        }
    }
    return name;
}
// TODO: at the moment, this function only knows how to serialize a table normally,
// but it should be equipped through a flag in ctx to serialize inline. (inline table)
//
// Instead of 
// [table_name]
// scalar_0 = 0
// scalar_1 = 1
//
// it would be:
// table_name = {scalar_0 = 0, scalar_1 = 1}
// NOTE: Recursive function
template<typename T>
[[nodiscard]] auto serialize_subtable_inline(
    SerializeContext<T> ctx
) -> std::string {
    // Non inlined subtables =
    // [NAME]
    // SCALAR_FIELDS = ...
    // NONSCALAR_FIELDS = [ {}... ]
}
template<typename T>
[[nodiscard]] auto serialize_table(
    SerializeContext<T> ctx
) -> std::string {
    auto res = std::string{};

    if (!ctx.inline_mode){
        ctx.nested_headers.emplace_back(ctx.key_identifier);
        res += serialize_header(ctx.nested_headers);
        // emits something like:
    } else{
        // emits something like: { field1 = 1, field2 = 2}
    }
    //NOTE:  IDEA: 
    // Separate out the loops into these three functions
    // The bottom two can recurse into this function.
    // Inline mode should make scalars  do:
    // IF inline mode: 
        // SCALARS (regular):
        // scalar1 = 1
        // scalar2 = 2
        // ---
        // SCALARS (inline):
        // scalar1 = 1, scalar2 = 2,
        //
    // inline mode should make nonscalars do:
        // NONSCALARS (regular):
        // [TABLE]
        // scalar_field = 1
        // [TABLE.non_scalar_field]
        // ...
        //
        // NONSCALARS (inline):
        // {scalar_field = 1, non_scalar_field = {...}}
    // inline mode doesnt change arrays serialization; it just activates it.
    res += serialize_scalars_of(v,ctx);
    res += serialize_nonscalars_of(v,ctx);
    res += serialize_arrays_of(v,ctx);
    return res;

    // NOTE: scalars 
    // Serialize them straight up, top level.
    template for (constexpr auto m: nonstatic_members_if([](auto m){
                  return is_serial_scalar(m);
    })){
        using U = typename [:meta::type_of(m):];
        // handle scalar case: simple
        constexpr auto scalar_key = get_identifier(m); 
        if (!ctx.inline_mode){
            res += serialize_scalar_no_inline<U>(ctx.value.[:m:],scalar_key);
        }else{
            res += serialize_scalar_inline<U>(ctx.value.[:m:], scalar_key);
        }

    }
    //NOTE: non-scalars (non arrays)
    template for (constexpr auto m: nonstatic_members_if(
        [](auto m){ return !is_serial_scalar(m) && is_nonscalar_array(m);}
    )){
        res += serialize_table(ctx);
    }
    // NOTE: arrays
    template for (constexpr auto m: nonstatic_members_if(
          [](auto m){ return !is_serial_scalar(m) && !is_nonscalar_array(m); }
    )){
        // recurse, but activate inline mode
    }
    return res + "\n";
}

template<typename T>
[[nodiscard]] auto serialize( T const& v ) -> std::vector<std::byte>{
    auto ctx = SerializeContext<T>{
        .value = v,
        .key_identifier = "",
        .nested_headers = {},
    };
    return to_byte_vector(serialize_table<T>(ctx));
}

// members which the predicate evaluates to true on are pushed to the front of the array

struct TripleInner{
    int a;
    int b;
};
struct DoubleInnerConfig{
    float even_more_inner;
    std::array<TripleInner,3> test;
};
struct InnerConfig{
    bool do_something_inner;
    DoubleInnerConfig inner_inner;
};

struct Finger{
    SRL_RENAME("FingerIdentifier") int finger_id;
    float tip_to_palm_cm;
};
struct Hand{
    std::array<Finger,5> fingers;
    float wrist_to_top_of_palm_cm;
};
struct Arm{
     Hand hand;
};
struct Person{
    std::array<Arm,2> arms;
    int age;
};
// A reflection comment declared as an annotation will appear as a comment in serialized config files
struct Config{
    // TODO: Walk the annotations and print their comments
    // Also add parsing
    SRL_DESC("Toggles wireframe rendering")     bool enable_wireframe;
    Person person;
};

// 1 Config file per struct
// Serializing a struct generates a file, where the root table represents that struct.
// The filename will be set based on the name provided
template<typename T>
auto serialize_and_save( T const& v, std::string_view filename) -> bool{
    if (filename.ends_with(".toml")){
        auto bytes = serialize<T>(v);
        save_to_file(fs::path(filename), bytes);
    }else{
        std::println(stderr, "Unable to parse file '{}' : unknown extension.",filename);
        return false;
    }
    return true;
}
auto run_tests() -> void{
    auto cfg = TestRootTable{};
    delete_file("config.toml");
    if (!serialize_and_save(cfg, "config.toml")){
        std::println(stderr, "Failed to serialize and save config.toml");
    }
}






//NOTE: Namespace refl END
} // leave this at the bottom of the file shmungus
