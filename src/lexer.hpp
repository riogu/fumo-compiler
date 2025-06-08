#include "token_definitions.hpp"
#include <fstream>
#include <vector>

#define punctuation_case(v_)                                    \
case static_cast<int>(Symbol::_##v_): {                         \
        tokens.push_back({.type = TokenType::_##v_,             \
                          .value = std::nullopt});              \
        break;                                                  \
}
#define operator_case(v_)                                       \
case static_cast<int>(Symbol::_##v_): {                         \
    tokens.push_back(                                           \
        file_stream.peek() == '='                               \
            ? Token {.type = TokenType::_##v_##_equals,         \
                     .value = std ::nullopt}                    \
            : Token {.type = TokenType::_##v_,                  \
                     .value = std ::nullopt});                  \
        break;                                                  \
}
#define ignore_case(v_)                                         \
case static_cast<int>(Symbol::_##v_): {                         \
    break;                                                      \
}

struct Lexer {
    i64 __FUMO_LINE_NUM__ = 1;
    char curr, start = 0;
    std::ifstream file_stream;

    [[nodiscard]] std::vector<Token> tokenize_file(const std::string file_name);
    [[nodiscard]] Result<Token, std::string> parse_numeric_literal();
    [[nodiscard]] bool identifier_ended();
};
