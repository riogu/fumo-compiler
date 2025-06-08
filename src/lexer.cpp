#include "lexer.hpp"
#include <fstream>

namespace Lexer {

using i64 = int64_t;
i64 __FUMO_LINE_NUM__ = 1;

[[nodiscard]] std::vector<Token> lex_file(const std::string file_name) {

    std::ifstream file_stream(file_name);
    std::string line;
    std::vector<Token> tokens;

    i64 start, curr = 0;

    while ((curr = file_stream.get()) != EOF) {
        switch (curr) {
            map_macro(punctuation_case, punctuations);
            map_macro(operator_case, operators);
            map_macro(ignore_case, ignores);
            // special cases
            case '\n':
                __FUMO_LINE_NUM__++;
                break;
            case '/':
                // handle comments or division
                if (file_stream.peek() == '/') {
                    while (file_stream.peek() != EOF && file_stream.peek() != '\n')
                        curr = file_stream.get();
                } else {
                    tokens.push_back(
                        Token {.type = TokenType::_division, .value = std::nullopt});
                }
                break;
            // TODO: literals
            // TODO: identifiers
            default:
                PANIC("provided unknown character.", char(curr));
        }
    }
    return tokens;
}

} // namespace Lexer
