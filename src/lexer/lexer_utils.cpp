#include "lexer.hpp"
#include <iostream>

#define symbol_case(v_) case int(Symbol::v_): return true;

[[nodiscard]] bool Lexer::identifier_ended() {
    switch (file_stream.peek()) {
        map_macro(symbol_case, all_symbols);
        default:
            return false;
    }
}


[[nodiscard]] str Lexer::peek_line() {
    std::string line;
    int len = file_stream.tellg();
    std::getline(file_stream, line);
    file_stream.seekg(len, std::ios_base::beg);
    return line;
}

int Lexer::get_curr() {
    __FUMO_LINE_OFFSET__++;
    curr = file_stream.get();
    return curr;
}

[[nodiscard]] Token Lexer::parse_identifier() {
    str value = std::format("{}", char(curr));

    while (!identifier_ended() && file_stream.peek() != EOF) {
        if (char next = get_curr(); std::isalnum(next) || next == '_') value += next;
        else
            lexer_error("Source file is not valid ASCII, or used unsupported character in identifier.");
    }

    return Token {.type = is_keyword(value) ? is_builtin_type(value) ? TokenType::builtin_type : TokenType::keyword
                                            : TokenType::identifier,
                  .literal = std::move(value),
                  .line_number = __FUMO_LINE_NUM__,
                  .line_offset = __FUMO_LINE_OFFSET__,
                  .file_offset = file_stream.tellg(),
                  .file_name = __FUMO_FILE__.string()};
}

[[nodiscard]] Token Lexer::parse_string_literal() {
    std::string str_inner{};
    get_curr();
    while (file_stream.peek() != EOF) {
        if (curr == '"') break;

        if (curr != '\\') {
            str_inner += curr;
        } else {
            if (file_stream.peek() != EOF) {
                get_curr();
            } else {
                lexer_error("Dangling backslash escape character.");
            }
            switch (curr) {
                case '0': str_inner += '\0'; break;
                case 'a': str_inner += '\a'; break;
                case 'r': str_inner += '\r'; break;
                case 'n': str_inner += '\n'; break;
                case 't': str_inner += '\t'; break;
                case '"': str_inner += '"'; break;
                case '\\': str_inner += '\\'; break;
                default: lexer_error("Invalid escape code.");
            }
        }

        get_curr();
    }
    if (curr != '"') {
        lexer_error("Unmatched quote in string literal.");
    }
    return Token { .type = TokenType::string,
                   .literal = std::move(str_inner),
                   .line_number = __FUMO_LINE_NUM__,
                   .line_offset = __FUMO_LINE_OFFSET__,
                   .file_offset = file_stream.tellg(),
                   .file_name = __FUMO_FILE__.string()};
}

[[nodiscard]] Token Lexer::parse_numeric_literal() {
    str value = std::format("{}", char(curr));
    Token token {.type = TokenType::integer};

    // FIXME: fix the syntax for floating point numbers to be compliant
    while ((!identifier_ended() || file_stream.peek() == '.')
           && file_stream.peek() != EOF) {

        get_curr();

        if (curr == '.' && std::isdigit(file_stream.peek())) {
            value += curr;
            token.type = TokenType::floating_point;
        } 
        else if (std::isdigit(curr))
            value += curr;
        else if (std::isalpha(curr)) {
            if (curr != 'f' || token.type != TokenType::floating_point)
                lexer_error("invalid digit '{}' in decimal constant. NOTE: (only 'f' is allowed in floats).",
                            char(curr));
        } else
            lexer_error("Source file is not valid ASCII.");
    }

    if (token.type == TokenType::integer) token.literal = std::stoi(value);
    else if (token.type == TokenType::floating_point)
        token.literal = std::stof(value);
    else
        token.literal = std::move(value);

    token.line_number = __FUMO_LINE_NUM__;
    token.line_offset = __FUMO_LINE_OFFSET__;
    token.file_offset = file_stream.tellg();
    token.file_name = __FUMO_FILE__.string();

    return token;
}
