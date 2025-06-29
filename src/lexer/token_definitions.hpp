// clang-format off
#pragma once
#include <libassert/assert.hpp>
#include <optional>
#include "utils/map-macro.hpp"
#include <string>
#include <variant>
#include <vector>
#include "symbol_definitions.hpp"

#define literals identifier, keyword, integer, floating_point, string
#define all_tokens punctuators, literals

namespace fs = std::filesystem; 
#define map_macro(macro, ...) MAP(macro, __VA_ARGS__)
#define fmt std::format
using i64 = int64_t;
template<typename  T> using Vec = std::vector<T>;

#define make_enum(_v) _v,
enum struct TokenType {map_macro(make_enum, all_tokens)};

using TokenValue = std::variant<int, double, std::string>;

struct Token {
    TokenType type;
    std::optional<TokenValue> value;
    i64 line_number, line_offset;


#define each_token(_v) case TokenType::_v: return all_token_strings.at(#_v);

    [[nodiscard]] inline constexpr str to_str() {
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

    [[nodiscard]] inline constexpr str type_name() {
        switch (type) {
            map_macro(tkntype, all_tokens);
            default: PANIC(fmt("provided unknown TokenType '{}'.", (int)type));
        }
    }
};



#undef each_token
#undef tkntype
#undef make_enum
