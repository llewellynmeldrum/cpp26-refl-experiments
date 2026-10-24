#pragma once 
#include <string>
#include <vector>
#include "cfg_metadata.hpp"
#include "cfg_registration.hpp"
#include "file_io.hpp"
#include "common.hpp"
#if CPP26
// --- formerly cfg_serialize.refl.hpp ---
#include <algorithm>
#include <meta>
#include <string>
#include <array>
#include <vector>
#include <ranges>

#include "cfg_helpers.hpp"
#include "toml_traits.hpp"
#include "cppslop.hpp"
#include "logger.hpp"

namespace cfg{
enum struct TomlBraceType : char{
    eNone=0,
    eBeginStruct = '{',
    eBeginArray  = '[',
    eEndStruct = '}',
    eEndArray  = ']'
};
struct LineCreateInfo{
    bool IsHeader           {};
    bool IsAssignment       {};
    bool ExtraNewlineSuffix    {};
    bool ExtraNewlinePrefix {};
    std::string Key         {};
    std::string Value       {};
    cfg::TomlBraceType Brace   {};
    std::string Separator   {};
    std::string Comment     {};

    decltype(auto) enableExtraNewlineSuffix()                { ExtraNewlineSuffix= true;  return *this;}
    decltype(auto) enableExtraNewlinePrefix()                { ExtraNewlinePrefix= true;  return *this;}
    decltype(auto) enableAssignment ()                    { IsAssignment = true;  return *this;}
    decltype(auto) enableHeader     ()                    { IsHeader     = true;  return *this;}
    decltype(auto) setBrace         (cfg::TomlBraceType  in) { Brace        = in;    return *this;}
    decltype(auto) setValue         (std::string_view in) { Value        = in;    return *this;}
    decltype(auto) setSeparator     (std::string_view in) { Separator    = in;    return *this;}
    decltype(auto) setComment       (std::string_view in) { Comment      = in;    return *this;}
    decltype(auto) setKey           (std::string_view in) { Key          = in;    return *this;}
};
struct Line{
    constexpr Line(LineCreateInfo c, std::size_t a_indent)
        :IsHeader    (c.IsHeader    )
        ,ExtraNewlineSuffix(c.ExtraNewlineSuffix)
        ,ExtraNewlinePrefix(c.ExtraNewlinePrefix)
        ,IsAssignment(c.IsAssignment)
        ,Key         (c.Key         )
        ,Value       (c.Value       )
        ,Brace       (c.Brace       )
        ,Separator   (c.Separator   )
        ,Comment     (c.Comment     )
        ,IndentLevel (a_indent      )
    {}
    bool IsHeader           {};
    bool ExtraNewlineSuffix    {};
    bool ExtraNewlinePrefix    {};
    bool IsAssignment       {};
    std::string Key         {};
    std::string Value       {};
    cfg::TomlBraceType Brace   {}; 
    std::string Separator   {};
    std::string Comment     {};
    std::size_t IndentLevel {};

    // Does not account for comment length or gap created for alignment
    constexpr auto content_length() const -> std::size_t{
        ASSERT(this != nullptr);
        return serialize_no_comment().size();
    }
    constexpr auto serialize_no_comment() const -> std::string{
        ASSERT(this != nullptr);
        auto repeat_string = [](std::string_view sv, std::size_t n){
            auto s = std::string{};
            while (n-->0) s+=std::string{sv};
            return s;
        };
        auto const indent = repeat_string("    ",IndentLevel);
        // [indent] [key] [assn ?' = ':''] [val] [brace] [sep] [comment]
        // Eg: 
        //         value    =
        return std::format(
            "{}{}{}{}{}{}",
            indent,
            IsHeader ? std::format("[{}]",Key) : Key,
            IsAssignment ? " = " : "",
            Value,
            (Brace != TomlBraceType::eNone) ?  std::string{std::to_underlying(Brace)} : "",
            Separator
        );
    }
    constexpr auto serialize(std::size_t comment_start_col) const -> std::string{
        ASSERT(this != nullptr);

        auto const non_comment_string = serialize_no_comment();
        auto const filler_len = comment_start_col+1 - non_comment_string.size(); // +1 col for # char
        auto const filler = std::string(std::max(0uz, filler_len), ' ');

        bool comment_on_previous_line = IsHeader || Brace == TomlBraceType::eBeginArray;
        if (comment_on_previous_line){
            return std::format(
                "{}{}\n{}{}",
                ExtraNewlinePrefix? "\n" : "",
                Comment.empty() ? "" : std::format("# {}",Comment),
                non_comment_string,
                ExtraNewlineSuffix ? "\n" : ""
            );
        }
        return std::format(
            "{}{}{}{}{}",
            ExtraNewlinePrefix ? "\n" : "",
            non_comment_string,
            filler,
            Comment.empty() ? "" : std::format("# {}",Comment),
            ExtraNewlineSuffix ? "\n" : ""
        );

    }
};

struct Writer{
    ssize_t current_indent {-1z};
    std::size_t max_line_length{0z};
    std::vector<Line> lines;
    auto push_scope(){ current_indent++; }
    auto pop_scope(){ current_indent--; }
    auto add_line(LineCreateInfo create_info) -> void{
        auto& line = lines.emplace_back(create_info, current_indent);
        max_line_length = std::max(max_line_length, line.content_length());
    }
    auto serialize() const -> std::string{
        auto res = std::string{};
        for (auto const& line: lines){
            auto const comment_start_col = max_line_length == 0 ? 0 : max_line_length + 1; // +1 for single space 
            res += line.serialize(comment_start_col) +"\n";
        }
        return res;
    }
};
template<std::meta::info field>
auto get_suffix_comment(std::string_view parent_name) -> std::string{
    using namespace std::meta;
    constexpr auto raw_type_name = stable_type_name_meta(meta::nocvref_alias(type_of(field)));
    using Ft = typename[:dealias(remove_cvref(type_of(field))):];
    constexpr auto has_range = try_get_range(field);
    constexpr auto has_note = try_get_note(field);
    auto type_str = std::string{};
    if constexpr (meta::is_struct_like(dealias(remove_cvref(type_of(field))))){
        type_str = std::string("struct ") + std::string(raw_type_name);
    }else{
        type_str = std::string(raw_type_name);
    }

    auto range_str = std::string{};
    if constexpr (has_range){
        auto r = *has_range;
        range_str = std::format("range=[{},{}]",r.min,r.max);
    }

    auto note_str = std::string{};
    if constexpr (has_note){
        auto r = *has_note;
        note_str = std::format("NOTE: '{}'",r.txt);
    }

    return std::format("{0}  {1}  {2}", type_str, range_str, note_str);
}
template<typename T>
struct serialize_context{
    T val;
    Writer & w;
    std::string_view identifier{""sv};
    std::string_view comment{""sv};
    bool parent_is_array{false};
    bool includeComma{false};
    bool parent_is_root{false};
    auto add_line(LineCreateInfo create_info){ w.add_line(create_info); }
};
template<typename T>
auto serialize_field(
    serialize_context<T> ctx
) -> void{
    if constexpr (concepts::is_scalar_type<T>){
        serialize_scalar_field<T>(ctx);
    }else if constexpr (concepts::is_struct_like<T>){
        serialize_struct_like_field<T>(ctx);
    }else {
        serialize_toml_array_field<T>(ctx);
    }
}
template<typename T>
auto serialize_scalar_field(
    serialize_context<T> ctx
) -> void{
    auto info = LineCreateInfo{}
        .setKey(ctx.identifier)
        .setValue(serialize_scalar_value(ctx.val))
        .setComment(ctx.comment)
    ;
    if (!ctx.parent_is_array){
        info.enableAssignment();
    }
    if (ctx.parent_is_array || ctx.includeComma){
        info.setSeparator(", "sv);
    }
    ctx.add_line(info);
}
template<typename T>
auto serialize_toml_array_field(
    serialize_context<T> ctx
) -> void{
    using namespace std::meta; 

    auto head_line_info = LineCreateInfo{}
        .setBrace(TomlBraceType::eBeginArray)
        .enableExtraNewlinePrefix()
        .setComment(ctx.comment)
    ;

    if (!ctx.parent_is_array){
        head_line_info
            .setKey(ctx.identifier)
            .enableAssignment()
        ;
    }
    
    auto tail_line_info = LineCreateInfo{}
        .setBrace(TomlBraceType::eEndArray)
    ;
    if (ctx.includeComma){
        tail_line_info
            .setSeparator(","sv);
    }

    ctx.add_line(head_line_info);
    ctx.w.push_scope();
    auto array_index = 0uz;
    for (auto const& element: ctx.val){
        using Ft = T::value_type;
        constexpr auto value_type_name = stable_type_name_meta(^^Ft);
        auto parent_name = ctx.identifier;
        serialize_field<Ft>({
            .val = element,
            .w = ctx.w,
            .identifier = ""sv,
            .comment = std::format(
                "{0} ({1}[{2}])",
                value_type_name, parent_name, array_index++
            ),
            .parent_is_array = true,
            .includeComma = true,
        });
    }
    ctx.w.pop_scope();

    ctx.add_line(tail_line_info);
}

template<typename T>
auto serialize_struct_like_field(
    serialize_context<T> ctx
) -> void{
    using namespace std::meta; 

    auto header_line_info = LineCreateInfo{}
        .setKey(ctx.identifier)
        .setComment(ctx.comment)
    ;

    if (ctx.parent_is_root){
        header_line_info
            .enableHeader()
            .enableExtraNewlinePrefix()
        ;
    }else {
        header_line_info
            .setBrace(TomlBraceType::eBeginStruct)
            ;
    }
    if(!ctx.parent_is_array && !ctx.parent_is_root){
        header_line_info
            .enableAssignment();
    }

    auto tail_line_info = LineCreateInfo{};
    if (ctx.includeComma){
        tail_line_info
            .setSeparator(","sv);
    }
    if (!ctx.parent_is_root){
        tail_line_info
            .setBrace(TomlBraceType::eEndStruct);
    }

    ctx.add_line(header_line_info);

    if (!ctx.parent_is_root){ ctx.w.push_scope(); }

    serialize_fields_of<T>(serialize_context<T>{
        .val = ctx.val,
        .w = ctx.w,
        .identifier = std::string(ctx.identifier),
        .comment = std::string(""),
        .parent_is_array = false,
        .includeComma = ctx.parent_is_array || (!ctx.parent_is_root),
        .parent_is_root = false,
    });

    if (!ctx.parent_is_root){
        ctx.w.pop_scope(); 
        ctx.add_line(tail_line_info); // If the parent is root, there is no tail line
    }

}
// called on struct like fields (such as the root, or structs which are arrays)
template<typename T>
auto serialize_fields_of( 
    serialize_context<T> ctx
) -> void{
    template for (constexpr auto field: cfg::meta::scalar_members_of<T>()){
        using Ft = typename [:remove_cvref(type_of(field)):];
        serialize_scalar_field<Ft>({ 
            .val = ctx.val.[:field:],
            .w = ctx.w,
            .identifier = has_identifier(field) ? identifier_of(field) : "?noID?"sv,
            .comment = get_suffix_comment<field>(ctx.identifier),
            .parent_is_array = false,
            .includeComma = ctx.includeComma, // might need to check if parent is root
            .parent_is_root = ctx.parent_is_root,
        });
    }
    template for (constexpr auto field: cfg::meta::array_like_members_of<T>()){
        using Ft = typename [:remove_cvref(type_of(field)):];
        serialize_toml_array_field<Ft>({ 
            .val = ctx.val.[:field:],
            .w = ctx.w,
            .identifier = has_identifier(field) ? identifier_of(field) : "?noID?"sv,
            .comment = get_suffix_comment<field>(ctx.identifier),
            .parent_is_array = false,
            .includeComma = ctx.includeComma,
            .parent_is_root = ctx.parent_is_root,
        });
    }
    template for (constexpr auto field: cfg::meta::struct_like_members_of<T>()){
        using Ft = typename [:remove_cvref(type_of(field)):];
        serialize_struct_like_field<Ft>({ 
            .val = ctx.val.[:field:],
            .w = ctx.w,
            .identifier = has_identifier(field) ? identifier_of(field) : "?noID?"sv,
            .comment = get_suffix_comment<field>(ctx.identifier),
            .parent_is_array = false,
            .includeComma = ctx.includeComma,
            .parent_is_root = ctx.parent_is_root,
        });
    }
}
template<typename T>
auto serialize_root_table(T const& root) -> std::string{
    static constexpr bool print_dbg_info = true;
    using namespace std::meta;
    using Reg = cfg::Registration<T>;

    auto const preface = std::format(
        "# === \n"
        "# struct {0}: {1}\n"
        "# ===\n",
        Reg::config_name,
        Reg::description
    );

    Writer w;
    w.push_scope();
    serialize_fields_of<T>(serialize_context<T>{
        .val = root,
        .w = w,
        .identifier = std::string(Reg::config_name),
        .comment = std::string(""),
        .parent_is_array = false,
        .includeComma = false,
        .parent_is_root = true,
    });
    w.pop_scope();
    if constexpr (print_dbg_info){
        LOG_DBG("Finished serializing '{}'."sv,Reg::config_name);
    }
    return std::format("{}\n\n{}",preface, w.serialize());
}

} // NOTE: namespace cfg
#endif // CPP26

namespace cfg{
// THINKING: 
// i want to have some sort of IR. 
// I thought about a sort of tree of FieldContext's, but its templated. 
// Could i remove the template somehow, but retain:
// - the default value
// What if instead, we walk through the 

// BRIEF: Serializes a given struct into TOML, using my own pretty opinionated style.
// See docs/toml_style.md for more information
// include/test_config.hpp
template<typename T>
auto serialize_toml(T const& root) -> std::vector<std::byte>{
    using Reg = cfg::Registration<T>;
    auto res = serialize_root_table(root);
    return io::to_byte_vector(res);
}
}// NOTE: namespace cfg
