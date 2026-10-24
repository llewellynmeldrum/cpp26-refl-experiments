#pragma once 
#include "meta_shorthand.hpp"
#include "common.hpp"
#if CPP26
#include <ranges>
#include <format>
#include "meta_helpers.hpp"
#include "annotation_helpers.hpp"
namespace refl{

struct auto_format_tag{};
inline constexpr auto auto_format = auto_format_tag{};
template<typename T>
concept auto_formattable = has_annotation<refl::auto_format_tag>(^^T);
#if !defined(CLANGD_PARSE)
#define AUTO_FORMAT [[=refl::auto_format]]
#else
#define AUTO_FORMAT 
#endif




struct auto_formatter;
} // NOTE: namespace refl
namespace refl{
struct auto_formatter{
    struct format_opts{
        bool subfields_print_types{false};
        bool include_static{false};
    }opts;

    // options: 
    // {:t} subfields show their type names
    // {:s} include static members
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        while (it != ctx.end() && *it != '}'){
            switch (*it){
                case 't': opts.subfields_print_types = true; break;
                case 's': opts.include_static= true; break;
                default: throw std::format_error("auto_formatter: Unknown option");
            }
            it++;
        }
        return it;
        return ctx.begin(); 
    }
    static constexpr auto apply_indent(auto& ctx, size_t indent)noexcept{
        auto out = std::format_to(ctx.out(),"");
        for (size_t i = 0; i<indent; i++){
            out = std::format_to(ctx.out(), "    ");
        }
        return out;
    };
    template<typename T>
    constexpr auto format_natively(
        T const& obj,
        std::string_view identifier,
        auto& ctx,
        format_opts const& opts,
        size_t indent
    )const noexcept{
        auto res = std::format_to(ctx.out(),"");
        if (opts.subfields_print_types){
            res = std::format_to(
                ctx.out(), ".{} : {} = {},",
                identifier,
                std::meta::display_string_of(^^T),
                obj
            );
        } else{
            res = std::format_to(
                ctx.out(), ".{} = {},",
                identifier,
                obj
            );
        }
        return res;
    }

    template<typename T>
    static constexpr auto format_field_name(
        T const& obj,
        std::string_view identifier,
        auto& ctx,
        format_opts const& opts
    )noexcept{
        if (opts.subfields_print_types){
            return std::format_to(
                ctx.out(), 
                "{}{} : {} {{\n",
                identifier=="" ? "" : ".",
                identifier,
                std::meta::display_string_of(^^T)
            );
        }else{
            return std::format_to(
                ctx.out(),
                "{}{} {{\n",
                identifier == "" ? "" : ".",
                identifier
            );
        }
    }
    template<typename T>
    constexpr auto recurse_format(
        T const& obj,
        std::string_view identifier,
        auto& ctx,
        format_opts const& opts,
        size_t indent,
        bool is_static
    )const noexcept{
        auto out = std::format_to(ctx.out(), "");
        // base case: formattable natively (i.e not one of ours)
        out = apply_indent(ctx, indent);
        if constexpr (!auto_formattable<T>){
            out = format_natively(obj, identifier, ctx,opts, indent+1);
        } else{
            if (is_static){
                out = std::format_to(ctx.out(), "(static)");
            }
            out = format_field_name(obj, identifier, ctx, opts);
            if (opts.include_static){
                template for (constexpr auto m: static_members_array<T>()){
                    using U = decltype(obj.[:m:]);
                    out = recurse_format<U>(obj.[:m:], std::meta::identifier_of(m), ctx, opts, indent+1,true);
                    out = std::format_to(ctx.out(), "\n");
                }
            }
            template for (constexpr auto m: nonstatic_members_array<T>()){
                using U = decltype(obj.[:m:]);
                out = recurse_format<U>(obj.[:m:], std::meta::identifier_of(m), ctx, opts, indent+1,false);
                out = std::format_to(ctx.out(), "\n");
            }
            out = apply_indent(ctx, indent);
            out = std::format_to(ctx.out(), "}}");
    //        out = std::format_to(ctx.out(), "}}\n");
        }
        return out;
    }


    template<typename T>
    constexpr auto format(const T& obj, auto& ctx) const noexcept{
        return recurse_format(obj,"",ctx,opts,0,false);
    }

};
} // NOTE: namespace refl
template<typename T> requires refl::auto_formattable<T>
struct std::formatter<T> : refl::auto_formatter { };
#endif // CPP26
