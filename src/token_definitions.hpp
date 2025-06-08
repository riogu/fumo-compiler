// clang-format off
#pragma once
#include "map-macro.hpp"
#include <optional>
#include <string>
#include <variant>
#include <libassert/assert.hpp>

using i64 = int64_t;
template<typename Ok, typename Err>
using Result = std::expected<Ok, Err>;
template<typename T>
using Err = std::unexpected<T>;
using str = std::string;
#define fmt std::format
//---------------------------------
// for switch case usage in lexer
enum struct Symbol {
// operator
    _equals        =   '=',
    _less          =   '<',
    _greater       =   '>',
    _exclamation   =   '!',
    _plus          =   '+',
    _minus         =   '-',
    _asterisk      =   '*',
// punctuation
    _open_bracket  =   '[',
    _close_bracket =   ']',
    _open_paren    =   '(',
    _close_paren   =   ')',
    _open_brace    =   '{',
    _close_brace   =   '}',
    _semicolon     =   ';',
// ignore
    _space         =   ' ',
    _tab           =   '\t',
    _r_thing       =   '\r',
};
//---------------------------------
// macros for nice printing
// operator
#define _repr_equals                "="
#define _repr_less                  "<"
#define _repr_greater               ">"
#define _repr_exclamation           "!"
#define _repr_plus                  "+"
#define _repr_minus                 "-"
#define _repr_asterisk              "*"

#define _repr_equals_equals         "=="
#define _repr_greater_equals        ">="
#define _repr_less_equals           "<="
#define _repr_exclamation_equals    "!="
#define _repr_plus_equals           "+="
#define _repr_minus_equals          "-="
#define _repr_asterisk_equals       "*="

// punctuation
#define _repr_open_paren            "("
#define _repr_close_paren           ")"
#define _repr_open_bracket          "["
#define _repr_close_bracket         "]"
#define _repr_open_brace            "{"
#define _repr_close_brace           "}"
#define _repr_semicolon             ";"
// special
#define _repr_division              "/"
#define _repr_comment               "//"
//---------------------------------
// language symbols and definitions
#define keywords        \
int,                    \
return

#define __operators     \
equals,                 \
less,                   \
greater,                \
exclamation,            \
plus,                   \
minus,                  \
asterisk                

#define operators __operators,  MAP_LIST(add_equals, __operators)
#define punctuations    \
open_bracket,           \
close_bracket,          \
open_paren,             \
close_paren,            \
open_brace,             \
close_brace,            \
semicolon               

#define ignores         \
space,                  \
tab,                    \
r_thing

#define literals identifier, keyword, integer, floating_point

#define add_equals(v_) v_##_equals

#define map_macro(macro, ...) MAP(macro, __VA_ARGS__)
#define all_symbols operators, punctuations, division, comment, literals

#define make_enum(_v) _##_v,
enum struct TokenType {map_macro(make_enum, all_symbols)};
#undef make_enum

using TokenValue = std::variant<int, double, std::string>;
struct Token {
    TokenType type;
    std::optional<TokenValue> value;
    i64 line_number, line_offset;
};

#define each_token(_v) case TokenType::_##_v: return _repr_##_v;

[[nodiscard]] inline std::string token_to_str(const Token& token) {
    switch (token.type) {
        map_macro(each_token, operators, punctuations, division, comment)
        case TokenType::_identifier:
        case TokenType::_keyword:
        case TokenType::_integer:
        case TokenType::_floating_point:
            return std::get<std::string>(token.value.value());
        default:
            PANIC(fmt("provided unknown TokenType '{}'.", (int)token.type));
    }
}

#undef each_token


#define tkntype(v_) case TokenType::_##v_: return #v_;

[[nodiscard]] inline str tokentype_name(const TokenType& type) {
    switch (type) {
        map_macro(tkntype, all_symbols);
        default: PANIC(fmt("provided unknown TokenType '{}'.", (int)type));
    }
}
#undef tkntype
