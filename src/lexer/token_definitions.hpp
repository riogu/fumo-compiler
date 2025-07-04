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

#define literals identifier, keyword, integer, floating_point, string
#define all_tokens punctuators, literals

namespace fs = std::filesystem; 
#define fmt std::format
using i64 = int64_t;
template<typename  T> using Vec = std::vector<T>;
template<typename T> using Opt = std::optional<T>;


#define make_enum(_v) _v,
enum struct TokenType {map_macro(make_enum, all_tokens)};


#define _make_hashmap1_(a, b) {TokenType::IDENTITY a, IDENTITY b},

const std::unordered_map<TokenType, std::string> all_token_strings {
    zip_to_macro(_make_hashmap1_, punctuators_, symbol_reprs_)
};

using TokenValue = std::variant<int, double, std::string>;

struct Token {
    TokenType type;
    std::optional<TokenValue> value;
    i64 line_number, line_offset, file_offset;
    std::string file_name;


#define each_token(_v) case TokenType::_v: return all_token_strings.at(TokenType::_v);

    [[nodiscard]] constexpr str to_str() {
        switch (type) {
            map_macro(each_token, punctuators)
            case TokenType::integer:
                return std::to_string(std::get<int>(value.value()));
            case TokenType::floating_point:
                return std::to_string(std::get<double>(value.value()));
            case TokenType::identifier: case TokenType::keyword: case TokenType::string:
                return std::get<std::string>(value.value());
            default: PANIC(fmt("provided unknown TokenType '{}'.", (int)type));
        }
    }

#define tkntype(v_) case TokenType::v_: return #v_;

    [[nodiscard]] constexpr str type_name() {
        switch (type) {
            map_macro(tkntype, all_tokens);
            default: PANIC(fmt("provided unknown TokenType '{}'.", (int)type));
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
    else PANIC(fmt("provided unknown token name: '{}'.", str));
}


#undef each_token
#undef tkntype
#undef make_enum
#undef _make_hashmap1_
#undef each_str_to_tkn
