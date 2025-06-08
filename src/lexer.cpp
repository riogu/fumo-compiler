#include "lexer.hpp"
#include <fstream>
#include <string>

#define fmt_error(error_msg)                                                    \
 fmt("\n\t| error at line {}:\n\t| {}\n\t|{}{}",                                \
    __FUMO_LINE_NUM__,                                                          \
    __FUMO_LINE__,                                                              \
    str(__FUMO_LINE_OFFSET__, ' ') + "^ ",                                      \
    error_msg                                                                   \
    )

[[nodiscard]] str Lexer::peek_line() {
    std::string line;
    int len = file_stream.tellg();
    std::getline(file_stream, line);
    file_stream.seekg(len, std::ios_base::beg);
    return line;
}

[[nodiscard]] char Lexer::get_curr() {
    __FUMO_LINE_OFFSET__++;
    char curr = file_stream.get();
    return curr;
}

[[nodiscard]] Result<std::vector<Token>, str> Lexer::tokenize_file(const str _file_name) {
    file_stream = std::ifstream(file_name);
    file_name = _file_name;
    std::vector<Token> tokens;
    __FUMO_LINE__ = peek_line();

    while ((curr = get_curr()) != EOF) {
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

        switch (curr) {
            map_macro(punctuation_case, punctuations);
            map_macro(operator_case, __operators);
            map_macro(ignore_case, ignores);
            // special cases
            case '/':
                // handle comments or division
                if (file_stream.peek() == '/') {
                    while (file_stream.peek() != EOF && file_stream.peek() != '\n')
                        curr = get_curr();
                } else {
                    tokens.push_back(Token {.type = TokenType::_division,
                                            .value = std::nullopt,
                                            .line_number = __FUMO_LINE_NUM__,
                                            .line_offset = __FUMO_LINE_OFFSET__});
                }
                break;
            case '\n':
                __FUMO_LINE_NUM__++;
                __FUMO_LINE_OFFSET__ = 0;
                __FUMO_LINE__ = peek_line();
                break;
            default:
                return Err(fmt_error("Source file is not valid ASCII."));
        }
    }
    return tokens;
}

#define symbol_case(v_) case static_cast<int>(Symbol::_##v_): return true;

[[nodiscard]] bool Lexer::identifier_ended() {
    switch (file_stream.peek()) {
        map_macro(symbol_case, __operators, punctuations, ignores);
        default:
            return false;
    }
}

#undef symbol_case

[[nodiscard]] Result<Token, str> Lexer::parse_numeric_literal() {
    str value = std::format("{}", curr);
    Token token {.type = TokenType::_integer};

    while (!identifier_ended()) {
        char next = get_curr();
        if ((next == '.' && std::isdigit(file_stream.peek()))) {
            value += next;
            token.type = TokenType::_floating_point;
        } //
        else if (std::isdigit(next))
            value += next; // continue getting number
        else if (std::isalpha(next))
            return Err(fmt_error(fmt("invalid digit '{}' in decimal constant.", next)));
        else
            return Err(fmt_error(fmt("unknown character '{}'.", next)));
    }

    token.value = value;
    token.line_number = __FUMO_LINE_NUM__;
    token.line_offset = __FUMO_LINE_OFFSET__;
    return token;
}
