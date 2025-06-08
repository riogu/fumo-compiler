#include "token_definitions.hpp"
#include <fstream>
#include <vector>

#define add_token_info                  \
.value = std::nullopt,                  \
.line_number = __FUMO_LINE_NUM__,       \
.line_offset = __FUMO_LINE_OFFSET__

#define fmt_error(error_msg)                                        \
 fmt("\n\t| error at line {}:\n\t| {}\n\t|{}{}",                    \
    __FUMO_LINE_NUM__,                                              \
    __FUMO_LINE__,                                                  \
    str(__FUMO_LINE_OFFSET__, ' ') + "^ ",                          \
    error_msg                                                       \
    )

#define punctuation_case(v_)                                        \
case static_cast<int>(Symbol::_##v_): {                             \
        tokens.push_back({.type = TokenType::_##v_,                 \
                          add_token_info});                         \
        break;                                                      \
}
#define operator_case(v_)                                           \
case static_cast<int>(Symbol::_##v_): {                             \
    tokens.push_back(                                               \
        file_stream.peek() == '='                                   \
            ? Token {.type = TokenType::_##v_##_equals,             \
                          add_token_info}                           \
            : Token {.type = TokenType::_##v_,                      \
                          add_token_info});                         \
        break;                                                      \
}
#define ignore_case(v_)                                             \
case static_cast<int>(Symbol::_##v_): {                             \
    break;                                                          \
}

struct Lexer {
    i64 __FUMO_LINE_NUM__ = 1;
    i64 __FUMO_LINE_OFFSET__ = 0;
    str __FUMO_LINE__;
    char curr = 0;
    std::ifstream file_stream;
    str file_name;

    [[nodiscard]] Result<std::vector<Token>, str> tokenize_file(const str file_name);
    [[nodiscard]] Result<Token, str> parse_numeric_literal();
    [[nodiscard]] Result<Token, str> parse_identifier();
    [[nodiscard]] str peek_line();
    [[nodiscard]] bool is_keyword(const str identifier);
    [[nodiscard]] bool identifier_ended();
    [[nodiscard]] char get_curr();
};
