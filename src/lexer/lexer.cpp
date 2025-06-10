// clang-format off
#include "lexer.hpp"
#include "symbol_cases.hpp"
#include "token_definitions.hpp"
#include <fstream>

#define add_token(tkn) tokens.push_back(Token {.type = TokenType::tkn, add_token_info})
#define add_and_consume_token(tkn) do {tokens.push_back(Token {.type = TokenType::tkn, add_token_info}); get_curr();} while(0)
#define next_is(tkn) (file_stream.peek() == tkn)
#define make_token(tkn) Token {.type = TokenType::tkn, add_token_info}
#define make_and_consume_token(tkn) ({get_curr(); Token {.type = TokenType::tkn, add_token_info};})

[[nodiscard]] Result<Vec<Token>, Str> Lexer::tokenize_file(const fs::path _file_name) {
    __FUMO_FILE__ = _file_name.filename(); __FUMO_LINE__ = peek_line();
    file_stream = std::ifstream(_file_name); Vec<Token> tokens;

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
            // FIXME: - fix issues with EOF during the lexe
            //        - add -> punctuator case
            //
            // -----------------------------------------------------------
            // handle each symbol based on its possible compound symbols
            cases(singular);
            cases(has_equals);
            cases(has_double_and_equals);
            cases(ignore);
            // -----------------------------------------------------------
            // triple cases are handled individually
            case '.':
                if ((file_stream.peek() == '.')) {
                    if (get_curr(); (file_stream.peek() == '.')) add_and_consume_token(dot_dot_dot);
                    else
                        PANIC(fmt_error("Expected complete '...' ellipsis."));
                } else 
                      add_token(dot);
                break;
            case '<':
                if (file_stream.peek() == '<') { // <<
                    if (get_curr(); file_stream.peek() == '=') // <<=
                        add_and_consume_token(less_less_equals);
                    else
                        add_and_consume_token(less_less);
                } else if (file_stream.peek() == '=') // <=
                    add_and_consume_token(less_equals);
                else // <
                    add_token(less);
                break;

            case '>':
                if (file_stream.peek() == '>') { // <<
                    if (get_curr(); file_stream.peek() == '=') // <<=
                        add_and_consume_token(greater_greater_equals);
                    else
                        add_and_consume_token(greater_greater);
                } else if (file_stream.peek() == '=') // <=
                    add_and_consume_token(greater_equals);
                else // <
                    add_token(greater);
                break;
            // -----------------------------------------------------------
            // special cases
            case '/':
                if (next_is('/'))
                    while (!next_is(EOF) && !next_is('\n')) curr = get_curr();
                else
                    tokens.push_back(next_is('=') ? make_and_consume_token(division_equals)
                                                  : make_token(division));
                break;

            case '\n': __FUMO_LINE_NUM__++; __FUMO_LINE_OFFSET__ = 0; __FUMO_LINE__ = peek_line(); break;

            case '#':
                tokens.push_back(next_is('#') ? make_and_consume_token(hashtag_hashtag)
                                              : make_token(hashtag));
                break;

            default: PANIC(fmt_error("Source file is not valid ASCII."));
        }
    }
    return tokens;
}

// clang-format off

[[nodiscard]] Result<Token, Str> Lexer::parse_identifier() {
    Str value = std::format("{}", char(curr));

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
    Str value = std::format("{}", char(curr));
    Token token {.type = TokenType::integer};

    while (!identifier_ended()) {
        if (char next = get_curr(); next == '.' && std::isdigit(file_stream.peek())) {
            value += next;
            token.type = TokenType::floating_point;
        } //
        else if (std::isdigit(next))
            value += next; // continue getting number
        else if (std::isalpha(next)) {
            if (next != 'f')
                return Err(fmt_error(fmt("invalid digit '{}' in decimal constant.", next)));
        } else
            return Err(fmt_error("Source file is not valid ASCII."));
    }

    token.value = value;
    token.line_number = __FUMO_LINE_NUM__;
    token.line_offset = __FUMO_LINE_OFFSET__;
    return token;
}
