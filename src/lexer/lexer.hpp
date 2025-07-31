#pragma once
#include "base_definitions/tokens.hpp"

struct Lexer {
    i64 __FUMO_LINE_NUM__ = 1;
    i64 __FUMO_LINE_OFFSET__ = 0;
    std::string __FUMO_LINE__;
    fs::path __FUMO_FILE__;
    std::stringstream file_stream;
    int curr = 0;

    [[nodiscard]] std::pair<vec<Token>, File> tokenize_file(const fs::path& _file_name);
    [[nodiscard]] std::pair<vec<Token>, File> tokenize_string(const std::string& testname,
                                                              const std::string& test_string);

  private:
    [[nodiscard]] vec<Token> tokenize();
    [[nodiscard]] Token parse_numeric_literal();
    [[nodiscard]] Token parse_identifier();
    [[nodiscard]] str peek_line();
    [[nodiscard]] bool identifier_ended();
    int get_curr();
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
.literal = std::nullopt,                                                \
.line_number = __FUMO_LINE_NUM__,                                       \
.line_offset = __FUMO_LINE_OFFSET__,                                    \
.file_offset = file_stream.tellg(),                                     \
.file_name = __FUMO_FILE__.string()

#define lexer_error(...)                                                \
{                                                                       \
    std::cerr << _lexer_error(__VA_ARGS__) << std::endl;                \
    std::exit(1);                                                       \
}

#define _lexer_error(...)                                               \
std::format("\n  | error in file '{}' at line {}:\n  | {}\n  |{}{}",    \
    __FUMO_FILE__.string(),                                             \
    __FUMO_LINE_NUM__,                                                  \
    __FUMO_LINE__,                                                      \
    std::string(__FUMO_LINE_OFFSET__, ' ') + "^ ",                      \
    std::format(__VA_ARGS__)                                            \
    )
