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

[[nodiscard]] Vec<Token> Lexer::tokenize_file(const fs::path& _file_name) {
    __FUMO_FILE__ = _file_name.filename(); __FUMO_LINE__ = peek_line();
    file_stream = std::ifstream(_file_name); Vec<Token> tokens;

    while ((curr = get_curr()) != EOF) {

        if (std::isdigit(curr)) {
            tokens.push_back(parse_numeric_literal());
            continue;
        }
        if (std::isalpha(curr) || curr == '_') {
            tokens.push_back(parse_identifier());
            continue;
        }
        switch (curr) {
            // FIXME: - fix issues with EOF during the lexer
            //        - convert the std::string ver of int/float literals to int/float
            //        - add string literal parsing
    
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
                if ((next_is('.'))) {
                    if (get_curr(); (next_is('.'))) add_and_consume_token(dot_dot_dot);
                    else
                        PANIC(fmt_error("Expected complete '...' ellipsis."));
                } else 
                      add_token(dot);
                break;
            case '<':
                if (next_is('<')) { // <<
                    if (get_curr(); next_is('=')) // <<=
                        add_and_consume_token(less_less_equals);
                    else
                        add_and_consume_token(less_less);
                } else if (next_is('=')) // <=
                    add_and_consume_token(less_equals);
                else // <
                    add_token(less);
                break;

            case '>':
                if (next_is('>')) { // <<
                    if (get_curr(); next_is('=')) // <<=
                        add_and_consume_token(greater_greater_equals);
                    else
                        add_and_consume_token(greater_greater);
                } else if (next_is('=')) // <=
                    add_and_consume_token(greater_equals);
                else // <
                    add_token(greater);
                break;
            // -----------------------------------------------------------
            // special cases
            case '-': 
                if(next_is('-')) 
                    add_and_consume_token(minus_minus); // -- 
                else if (next_is('='))
                    add_and_consume_token(minus_equals); // -=
                else if (next_is('>'))
                    add_and_consume_token(minus_greater); // ->
                else
                    add_token(minus);

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
