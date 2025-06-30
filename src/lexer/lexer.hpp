#pragma once

#include "token_definitions.hpp"
#include <fstream>

#define fmt_error(...)                                                \
std::format("\n  | error in file '{}' at line {}:\n  | {}\n  |{}{}",  \
    __FUMO_FILE__,                                                    \
    __FUMO_LINE_NUM__,                                                \
    __FUMO_LINE__,                                                    \
    std::string(__FUMO_LINE_OFFSET__, ' ') + "^ ",                    \
    std::format(__VA_ARGS__)                                          \
    )

struct Lexer {
    i64 __FUMO_LINE_NUM__ = 1;
    i64 __FUMO_LINE_OFFSET__ = 0;
    std::string __FUMO_LINE__, __FUMO_FILE__;
    std::ifstream file_stream;
    i64 curr = 0;

    [[nodiscard]] Result<Vec<Token>, str> tokenize_file(const fs::path& _file_name);

  private:
    [[nodiscard]] Token parse_numeric_literal();
    [[nodiscard]] Token parse_identifier();
    [[nodiscard]] bool is_keyword(const str identifier);
    [[nodiscard]] str peek_line();
    [[nodiscard]] bool identifier_ended();
    i64 get_curr();
};

// clang-format off

#define cases(symbols) map_macro(symbols##_case, symbols)

#define singular_case(tkn)                                              \
case (int)(Symbol::tkn): {                                              \
        tokens.push_back({.type = TokenType::tkn, add_token_info});     \
        break;                                                          \
}
#define has_equals_case(tkn)                                            \
case (int)Symbol::tkn: {                                                \
    tokens.push_back(                                                   \
        file_stream.peek() == '='                                       \
            ? ({get_curr();                                             \
              Token {.type = TokenType::tkn##_equals,add_token_info};}) \
            : Token {.type = TokenType::tkn, add_token_info});          \
        break;                                                          \
}
#define has_double_and_equals_case(tkn)                                 \
case (int)Symbol::tkn: {                                                \
    tokens.push_back(                                                   \
        file_stream.peek() == (int)Symbol::tkn                          \
            ? ({get_curr();                                             \
                Token {.type =TokenType::tkn##_##tkn,add_token_info};}) \
            : file_stream.peek() == '=' ?                               \
              ({get_curr();                                             \
              Token {.type =TokenType::tkn##_equals,add_token_info};})  \
            : Token {.type =TokenType::tkn, add_token_info});           \
    break;                                                              \
}
#define ignore_case(tkn)                                                \
case static_cast<int>(Symbol::tkn): {                                   \
    break;                                                              \
}

#define add_token_info                                                  \
.value = std::nullopt,                                                  \
.line_number = __FUMO_LINE_NUM__,                                       \
.line_offset = __FUMO_LINE_OFFSET__

