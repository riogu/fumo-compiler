#include "token_definitions.hpp"
#include <vector>

namespace Lexer {

#define punctuation_case(v_)                                    \
case static_cast<int>(Symbols::_##v_): {                        \
        tokens.push_back({.type = TokenType::_##v_,             \
                          .value = std::nullopt});              \
        break;                                                  \
}
#define operator_case(v_)                                       \
case static_cast<int>(Symbols::_##v_): {                        \
    tokens.push_back(                                           \
        file_stream.peek() == '='                               \
            ? Token {.type = TokenType::_##v_##_equals,         \
                     .value = std ::nullopt}                    \
            : Token {.type = TokenType::_##v_,                  \
                     .value = std ::nullopt});                  \
        break;                                                  \
}
#define ignore_case(v_)                                         \
case static_cast<int>(Symbols::_##v_): {                        \
    break;                                                      \
}

[[nodiscard]] std::vector<Token> lex_file(const std::string file_name);

} // namespace Lexer
