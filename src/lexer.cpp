#include "lexer.hpp"
#include <fstream>

[[nodiscard]] Result<std::vector<Token>, str> Lexer::tokenize_file(const str file_name) {
    file_stream = std::ifstream(file_name);
    std::vector<Token> tokens;

    while ((curr = file_stream.get()) != EOF) {
        // int epic = 123123;
        // float epic = 1231.3244;
        if (std::isdigit(curr)) {

            auto token = parse_numeric_literal();

            if (token) tokens.push_back(token.value());
            else
                PANIC(token.error());
            continue;
        }
        if (std::isalpha(curr)) {
            //
            continue;
        }

        // clang-format off
        switch (curr) {
            map_macro(punctuation_case, punctuations);
            map_macro(operator_case, operators);
            map_macro(ignore_case, ignores);
            // special cases
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
            case '\n': __FUMO_LINE_NUM__++; break;
            default: return Err(fmt("provided unknown character '{}'.", char(curr)));
        }
    }
    return tokens;
}

#define symbol_case(v_) case static_cast<int>(Symbol::_##v_): return true;

[[nodiscard]] bool Lexer::identifier_ended() {
    switch (file_stream.peek()) {
        map_macro(symbol_case, operators, punctuations, ignores);
        default:
            return false;
    }
}

#undef symbol_case

[[nodiscard]] Result<Token, std::string> Lexer::parse_numeric_literal() {
    std::string value = std::format("{}", curr);
    Token token {.type = TokenType::_integer};

    while (!identifier_ended()) {
        char next = file_stream.get();
        if ((next == '.' && std::isdigit(file_stream.peek()))) {
            value += next;
            token.type = TokenType::_floating_point;
        } //
        else if (std::isdigit(next))
            value += next; // continue getting number
        else if (std::isalpha(next))
            return Err(std::format("invalid digit '{}' in decimal constant.", next));
        else
            return Err(std::format("unknown character '{}'.", next));
    }

    // token.value = (token.type == TokenType::_integer) ? std::atoi(value.c_str())
    //                                                   : std::atof(value.c_str());
    token.value = value;
    return token;
}
