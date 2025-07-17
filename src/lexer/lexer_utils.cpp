#include "lexer.hpp"
#include "lexer/token_definitions.hpp"
#include <iostream>

#define symbol_case(v_) case int(Symbol::v_): return true;

[[nodiscard]] bool Lexer::identifier_ended() {
    switch (file_stream.peek()) {
        map_macro(symbol_case, all_symbols);
        default:
            return false;
    }
}

#define check_keyword(v_) if(identifier == #v_) return true; else

[[nodiscard]] bool Lexer::is_keyword(const str identifier) {
    map_macro(check_keyword, keywords)
    return false;
}
[[nodiscard]] bool Lexer::is_builtin_type(const str identifier) {
    map_macro(check_keyword, builtin_types)
    return false;
}

#undef symbol_case
#undef check_keyword

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
            lexer_error("Source file is not valid ASCII or used unsupported character in identifier.");
    }

    return Token {.type = is_keyword(value) ? is_builtin_type(value) ? TokenType::builtin_type : TokenType::keyword
                                            : TokenType::identifier,
                  .value = std::move(value),
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

    if (token.type == TokenType::integer) token.value = std::stoi(value);
    else if (token.type == TokenType::floating_point)
        token.value = std::stof(value);
    else
        token.value = std::move(value);

    token.line_number = __FUMO_LINE_NUM__;
    token.line_offset = __FUMO_LINE_OFFSET__;
    token.file_offset = file_stream.tellg();
    token.file_name = __FUMO_FILE__.string();

    return token;
}
