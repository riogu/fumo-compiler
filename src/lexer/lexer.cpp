#include "lexer.hpp"
#include <fstream>
#include <iostream>

#define add_token(tok) tokens.push_back(Token {.type = tkn(tok), add_token_info})
#define add_and_consume_token(tok) do {tokens.push_back(Token {.type = tkn(tok), add_token_info}); get_curr();} while(0)
#define next_char_is(tok) (file_stream.peek() == 0[#tok])
#define make_token(tok) Token {.type = tkn(tok), add_token_info}
#define make_and_consume_token(tok) ({get_curr(); Token {.type = tkn(tok), add_token_info};})

[[nodiscard]] std::pair<vec<Token>, File>  Lexer::tokenize_file(const fs::path& _file_name) {
    __FUMO_FILE__ = _file_name;
    file_stream << std::ifstream(_file_name).rdbuf();
    return {tokenize(), File{_file_name, file_stream.str()}};
}
[[nodiscard]] std::pair<vec<Token>, File> Lexer::tokenize_string(const std::string& testname, const std::string& test_string) {
    __FUMO_FILE__ = testname;
    file_stream << test_string;
    return {tokenize(), File{testname, file_stream.str()}};
}

[[nodiscard]] vec<Token> Lexer::tokenize() {
    vec<Token> tokens;
    __FUMO_LINE__ = peek_line();

    while ((curr = get_curr()) != EOF) {

        if (curr == '\'') {
            tokens.push_back(parse_character_literal());
            continue;
        }

        if (curr == '"') {
            tokens.push_back(parse_string_literal());
            continue;
        }

        if (std::isdigit(curr)) {
            tokens.push_back(parse_numeric_literal());
            continue;
        }
        if (std::isalpha(curr) || curr == '_') {
            tokens.push_back(parse_identifier());
            continue;
        }
        switch (curr) {
            //
            // -----------------------------------------------------------
            // handle each symbol based on its possible compound symbols
            cases(singular);
            cases(has_equals);
            cases(has_double_and_equals);
            cases(ignore);
            // -----------------------------------------------------------
            // triple cases are handled individually
            case ':':
                tokens.push_back(next_char_is(:) ? make_and_consume_token(::)
                                                 : make_token(:));
                break;
            case '.':
                if ((next_char_is(.))) {
                    if (get_curr(); (next_char_is(.))) add_and_consume_token(...);
                    else
                        lexer_error("Expected complete '...' ellipsis.");
                } else 
                      add_token(.);
                break;
            case '<':
                if (next_char_is(<)) { 
                    if (get_curr(); next_char_is(=)) 
                        add_and_consume_token(<<=);
                    else
                        add_and_consume_token(<<);
                } else if (next_char_is(=)) 
                    add_and_consume_token(<=);
                else 
                    add_token(<);
                break;

            case '>':
                if (next_char_is(>)) { 
                    if (get_curr(); next_char_is(=)) 
                        add_and_consume_token(>>=);
                    else
                        add_and_consume_token(>>);
                } else if (next_char_is(=)) 
                    add_and_consume_token(>=);
                else 
                    add_token(>);
                break;
            // -----------------------------------------------------------
            // special cases
            case '-': 
                if(next_char_is(-)) 
                    add_and_consume_token(--);
                else if (next_char_is(=))
                    add_and_consume_token(-=);
                else if (next_char_is(>))
                    add_and_consume_token(->); 
                else
                    add_token(-);
                break;

            case '/':
                if (next_char_is(/))
                    while (file_stream.peek() != EOF && !next_char_is(\n)) curr = get_curr();
                else
                    tokens.push_back(next_char_is(=) ? make_and_consume_token(/=)
                                                     : make_token(/));
                break;

            case '#':
                tokens.push_back(next_char_is(#) ? make_and_consume_token(##)
                                                 : make_token(#));
                break;

            case '\n': __FUMO_LINE_NUM__++; __FUMO_LINE_OFFSET__ = 0; __FUMO_LINE__ = peek_line(); break;

            default: lexer_error("Source file is not valid ASCII.");
        }
    }
    add_token(is_EOF);
    return tokens;
}

#undef cases
