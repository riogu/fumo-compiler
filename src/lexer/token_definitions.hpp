// clang-format off
#pragma once
#include <libassert/assert.hpp>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include <vector>
#include "symbol_definitions.hpp"


namespace fs = std::filesystem; 

using i64 = int64_t;
template<typename  T> using Vec = std::vector<T>;
template<typename T> using Opt = std::optional<T>;


#define make_enum_member(_v) _v,
enum struct TokenType {map_macro(make_enum_member, all_tokens)};


#define _make_hashmap1_(a, b) {TokenType::IDENTITY a, IDENTITY b},

const std::unordered_map<TokenType, std::string> all_token_strings {
    zip_to_macro(_make_hashmap1_, punctuators_, symbol_reprs_)
};

using Literal = std::variant<int, double, std::string>;

#define int_pink(str) "\033[38;2;224;180;187m" + str + "\033[0m"
#define id_gold(str) "\033[38;2;252;191;85m"+ str + "\033[0m"
#define token_grey_green(str) "\033[38;2;140;170;190m" + str + "\033[0m"

struct Token {
    TokenType type;
    std::optional<Literal> literal;
    i64 line_number, line_offset, file_offset;
    std::string file_name;

    #define each_token(_v) case TokenType::_v: return token_grey_green(all_token_strings.at(TokenType::_v));
    [[nodiscard]] constexpr str to_str() {
        switch (type) {
            map_macro(each_token, punctuators)
            case TokenType::integer:
                return int_pink(std::to_string(std::get<int>(literal.value())));
            case TokenType::floating_point:
                return std::to_string(std::get<double>(literal.value()));
            case TokenType::identifier: case TokenType::keyword: case TokenType::builtin_type: case TokenType::string:
                return id_gold(std::get<std::string>(literal.value()));
            case TokenType::is_EOF:
                return "EOF";
            default: 
                PANIC(std::format("provided unknown TokenType '{}'.", (int)type));
        }
    }

// \033[38;2;252;191;85m{}\033[0m 
// gold color
//\033[38;2;156;209;255m{}\033[0m
//blue color

#define tkntype(v_) case TokenType::v_: return #v_;

    [[nodiscard]] constexpr str type_name() {
        switch (type) {
            map_macro(tkntype, all_tokens);
            default: PANIC(std::format("provided unknown TokenType '{}'.", (int)type));
        }
    }
};


// hacky conversions here
#define each_str_to_tkn(a, b) if(str == IDENTITY a) {return TokenType::IDENTITY b;} else
#define each_literal_to_tkn(a) if(str == #a) {return TokenType::a;} else

#define tkn(tok) str_to_tkn_type(#tok)
[[nodiscard]] constexpr TokenType str_to_tkn_type(std::string_view str) {
    zip_to_macro(each_str_to_tkn, symbol_reprs_, punctuators_)
    if (str == "keyword") return TokenType::keyword;
    else if (str == "int") return TokenType::integer;
    else if (str == "float") return TokenType::floating_point;
    else if (str == "string") return TokenType::string;
    else if (str == "identifier") return TokenType::identifier;
    else if(str == "is_EOF") return TokenType::is_EOF;
    else if(str == "builtin_type") return TokenType::builtin_type;
    else PANIC(std::format("provided unknown token name: '{}'.", str));
}


#undef each_token
#undef tkntype
#undef _make_hashmap1_
#undef each_str_to_tkn
#undef make_enum_member
