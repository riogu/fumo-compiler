// clang-format off
#include "lexer.hpp"
#include "symbol_cases.hpp"
#include "token_definitions.hpp"
#include <fstream>

[[nodiscard]] Result<Vec<Token>, Str> Lexer::tokenize_file(const fs::path _file_name) {
    __FUMO_FILE__ = _file_name.filename();
    __FUMO_LINE__ = peek_line();
    file_stream = std::ifstream(_file_name);
    Vec<Token> tokens;

    while ((curr = get_curr()) != EOF) {

        if (std::isdigit(curr)) {
            if (auto token = parse_numeric_literal()) tokens.push_back(token.value());
            else
                PANIC(token.error());
            continue;
        }
        if (std::isalpha(curr) || curr == '_') {
            if (auto token = parse_identifier()) tokens.push_back(token.value());
            else
                PANIC(token.error());
            continue;
        }
        switch (curr) {
            // -----------------------------------------------------------
            // handle each symbol based on its possible compound symbols
            cases(singular);
            cases(has_equals);
            cases(has_double_and_equals);
            cases(has_triple);
            cases(ignore);
            // -----------------------------------------------------------
            // special cases
            case '/':
                if (file_stream.peek() == '/') 
                    while (file_stream.peek() != EOF && file_stream.peek() != '\n') curr = get_curr();
                else 
                    tokens.push_back(Token {.type = TokenType::division, add_token_info});
                break;

            case '\n':
                __FUMO_LINE_NUM__++; __FUMO_LINE_OFFSET__ = 0; __FUMO_LINE__ = peek_line();
                break;

            case '#':
                tokens.push_back(
                    file_stream.peek() == '#'
                        ? Token {.type = TokenType::hashtag_hashtag, add_token_info}
                        : Token {.type = TokenType::hashtag, add_token_info});
                break;

            default: PANIC(fmt_error("Source file is not valid ASCII."));
        }
    }
    return tokens;
}

// clang-format on

[[nodiscard]] Result<Token, Str> Lexer::parse_identifier() {
    Str value = std::format("{}", curr);

    while (!identifier_ended()) {
        if (char next = get_curr(); std::isalnum(next) || next == '_') value += next;
        else
            return Err(fmt_error("Source file is not valid ASCII."));
    }
    return Token {.type = is_keyword(value) ? TokenType::keyword : TokenType::identifier,
                  .value = value,
                  .line_number = __FUMO_LINE_NUM__,
                  .line_offset = __FUMO_LINE_OFFSET__};
}

[[nodiscard]] Result<Token, Str> Lexer::parse_numeric_literal() {
    Str value = std::format("{}", curr);
    Token token {.type = TokenType::integer};

    while (!identifier_ended()) {
        char next = get_curr();
        if ((next == '.' && std::isdigit(file_stream.peek()))) {
            value += next;
            token.type = TokenType::floating_point;
        } //
        else if (std::isdigit(next))
            value += next; // continue getting number
        else if (std::isalpha(next)) {
            if (next != 'f')
                return Err(
                    fmt_error(fmt("invalid digit '{}' in decimal constant.", next)));
        } else
            return Err(fmt_error("Source file is not valid ASCII."));
    }

    token.value = value;
    token.line_number = __FUMO_LINE_NUM__;
    token.line_offset = __FUMO_LINE_OFFSET__;
    return token;
}
