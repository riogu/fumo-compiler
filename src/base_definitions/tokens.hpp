#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include <variant>
#include "utils/common_utils.hpp"
#include "symbols.hpp"
#include "utils/zip-macro.hpp"
#include <libassert/assert.hpp>

#define _make_hashmap1_(a, b) {TokenType::IDENTITY a, IDENTITY b},
#define make_enum_member(_v) _v,

enum struct TokenType {map_macro(make_enum_member, all_tokens)};

const std::unordered_map<TokenType, std::string> all_token_strings {
    zip_to_macro(_make_hashmap1_, punctuators_, symbol_reprs_)
};

using Literal = std::variant<int64_t, double, std::string, bool>;


#define make_color(rgb, symbol) std::string("\033[38;2;" #rgb "m") + std::string(symbol) + std::string("\033[0m")
#define int_pink(symbol)         make_color(224;180;187, symbol)
#define id_gold(symbol)          make_color(252;191;85,  symbol)  
#define token_grey_green(symbol) make_color(140;170;190, symbol) 
#define gray(symbol)             make_color(134;149;179, symbol) 
#define yellow(symbol)           make_color(252;191;85,  symbol)  
#define blue(symbol)             make_color(156;209;255, symbol) 
#define purple_blue(symbol)      make_color(142;163;217, symbol) 
#define enum_green(symbol)       make_color(88;154;143,  symbol)  
#define white_gray(symbol)       make_color(205;212;232, symbol) 
#define red(symbol)              make_color(235;67;54,   symbol)   
#define green(symbol)            make_color(88;154;143,  symbol)  




struct Token {
    TokenType type;
    std::optional<Literal> literal;
    i64 line_number, line_offset, file_offset;
    std::string file_name;

    #define each_token(_v) case TokenType::_v: return token_grey_green(all_token_strings.at(TokenType::_v));
    [[nodiscard]] constexpr std::string to_str() const {
        switch (type) {
            map_macro(each_token, punctuators)
            case TokenType::integer:
                return int_pink(std::to_string(std::get<int64_t>(literal.value())));
            case TokenType::floating_point:
                return int_pink(std::to_string(std::get<double>(literal.value())));
            case TokenType::identifier:
                return white_gray(std::get<std::string>(literal.value()));
            case TokenType::keyword:
                return purple_blue(std::get<std::string>(literal.value()));
            case TokenType::builtin_type: 
                return id_gold(std::get<std::string>(literal.value()));
            case TokenType::string: {
                std::string raw{std::get<std::string>(literal.value())};
                std::string escaped {};
                escaped.reserve(raw.length());
                for (char &c : raw) {
                    escaped += (c == '\n') ? "\\n" :
                               (c == '\t') ? "\\t" :
                               std::string(1, c);
                }
                return enum_green(escaped);
            }
            case TokenType::char_literal: {
                str temp = "";
                temp += std::get<int64_t>(literal.value());
                return enum_green(temp);
            }

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

    [[nodiscard]] constexpr std::string type_name() const {
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
    if (str == "<") {
        return TokenType ::less;
    } else if (str == ">") {
        return TokenType ::greater;
    } else if (str == "!") {
        return TokenType ::exclamation;
    } else if (str == "+") {
        return TokenType ::plus;
    } else if (str == "-") {
        return TokenType ::minus;
    } else if (str == "*") {
        return TokenType ::asterisk;
    } else if (str == "&") {
        return TokenType ::ampersand;
    } else if (str == "~") {
        return TokenType ::negate;
    } else if (str == "%") {
        return TokenType ::percent;
    } else if (str == "#") {
        return TokenType ::hashtag;
    } else if (str == "^") {
        return TokenType ::hat;
    } else if (str == "|") {
        return TokenType ::bar;
    } else if (str == "/") {
        return TokenType ::division;
    } else if (str == "=") {
        return TokenType ::equals;
    } else if (str == "?") {
        return TokenType ::question_mark;
    } else if (str == ":") {
        return TokenType ::colon;
    } else if (str == ".") {
        return TokenType ::dot;
    } else if (str == "[") {
        return TokenType ::open_bracket;
    } else if (str == "]") {
        return TokenType ::close_bracket;
    } else if (str == "(") {
        return TokenType ::open_paren;
    } else if (str == ")") {
        return TokenType ::close_paren;
    } else if (str == "{") {
        return TokenType ::open_brace;
    } else if (str == "}") {
        return TokenType ::close_brace;
    } else if (str == ";") {
        return TokenType ::semicolon;
    } else if (str == ",") {
        return TokenType ::comma;
    } else if (str == "==") {
        return TokenType ::equals_equals;
    } else if (str == ">=") {
        return TokenType ::greater_equals;
    } else if (str == "<=") {
        return TokenType ::less_equals;
    } else if (str == "!=") {
        return TokenType ::exclamation_equals;
    } else if (str == "+=") {
        return TokenType ::plus_equals;
    } else if (str == "-=") {
        return TokenType ::minus_equals;
    } else if (str == "*=") {
        return TokenType ::asterisk_equals;
    } else if (str == "/=") {
        return TokenType ::division_equals;
    } else if (str == "%=") {
        return TokenType ::percent_equals;
    } else if (str == "&=") {
        return TokenType ::ampersand_equals;
    } else if (str == "^=") {
        return TokenType ::hat_equals;
    } else if (str == "|=") {
        return TokenType ::bar_equals;
    } else if (str == "||") {
        return TokenType ::bar_bar;
    } else if (str == "--") {
        return TokenType ::minus_minus;
    } else if (str == "++") {
        return TokenType ::plus_plus;
    } else if (str == "##") {
        return TokenType ::hashtag_hashtag;
    } else if (str == "&&") {
        return TokenType ::ampersand_ampersand;
    } else if (str == ">>") {
        return TokenType ::greater_greater;
    } else if (str == "<<") {
        return TokenType ::less_less;
    } else if (str == "<<=") {
        return TokenType ::less_less_equals;
    } else if (str == ">>=") {
        return TokenType ::greater_greater_equals;
    } else if (str == "...") {
        return TokenType ::dot_dot_dot;
    } else if (str == "->") {
        return TokenType ::minus_greater;
    } else if (str == "::") {
        return TokenType ::colon_colon;
    } else if (str == "\"") {
        return TokenType ::quotation_mark;
    } else if (str == "keyword")
        return TokenType::keyword;
    if (str == "int")           return TokenType::integer;
    if (str == "float")         return TokenType::floating_point;
    if (str == "string")        return TokenType::string;
    if (str == "char_literal")  return TokenType::char_literal;
    if (str == "identifier")    return TokenType::identifier;
    if (str == "is_EOF")        return TokenType::is_EOF;
    if (str == "builtin_type")  return TokenType::builtin_type;
    PANIC(std::format("provided unknown token name: '{}'.", str));
}

#define check_keyword(v_) if(identifier == #v_) return true;

[[nodiscard]] inline bool is_keyword(const str identifier) {
    map_macro(check_keyword, keywords)
    return false;
}
[[nodiscard]] inline bool is_builtin_type(const str identifier) {
    map_macro(check_keyword, builtin_types)
    return false;
}

#undef symbol_case
#undef check_keyword

#undef each_token
#undef tkntype
#undef _make_hashmap1_
#undef each_str_to_tkn
#undef make_enum_member
