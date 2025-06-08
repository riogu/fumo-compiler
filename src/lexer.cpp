#include "lexer.hpp"
#include "token_definitions.hpp"
#include <fstream>
#include <string>

[[nodiscard]] Result<std::vector<Token>, str> Lexer::tokenize_file(const str _file_name) {
    file_stream = std::ifstream(_file_name);
    file_name = _file_name;
    std::vector<Token> tokens;
    __FUMO_LINE__ = peek_line();

    while ((curr = get_curr()) != EOF) {
        if (std::isdigit(curr)) {

            auto token = parse_numeric_literal();

            if (token) tokens.push_back(token.value());
            else
                PANIC(token.error());
            continue;
        }
        if (std::isalpha(curr)) {
            auto token = parse_identifier();
            if (token) tokens.push_back(token.value());
            else
                PANIC(token.error());
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
                    tokens.push_back(
                        Token {.type = TokenType::_division, add_token_info});
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

[[nodiscard]] bool Lexer::identifier_ended() {
#define symbol_case(v_) case static_cast<int>(Symbol::_##v_): return true;
    switch (file_stream.peek()) {
        map_macro(symbol_case, __operators, punctuations, ignores);
        default:
            return false;
    }
#undef symbol_case
}

[[nodiscard]] bool Lexer::is_keyword(const str identifier) {
#define check_keyword(v_) if(identifier == #v_) return true;
    map_macro(check_keyword, keywords);
    return false;
#undef check_keyword
}

[[nodiscard]] Result<Token, str> Lexer::parse_identifier() {
    str value = std::format("{}", curr);
    Token token {.type = TokenType::_integer};

    while (!identifier_ended()) {
        char next = get_curr();
        if (std::isalpha(next) || std::isdigit(next)) value += next;
        else
            return Err(fmt_error(fmt("unknown character '{}'.", next)));
    }

    token.type = is_keyword(value) ? TokenType::_keyword : TokenType::_identifier;
    token.value = value;
    token.line_number = __FUMO_LINE_NUM__;
    token.line_offset = __FUMO_LINE_OFFSET__;
    return token;
}

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
