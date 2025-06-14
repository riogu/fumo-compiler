#include "lexer.hpp"
#include "lexer/token_definitions.hpp"
#include <iostream>

#define symbol_case(v_) case static_cast<int>(Symbol::v_): return true;

[[nodiscard]] bool Lexer::identifier_ended() {
    switch (file_stream.peek()) {
        map_macro(symbol_case, all_symbols);
        default:
            return false;
    }
}

#define check_keyword(v_) if(identifier == #v_) return true;

[[nodiscard]] bool Lexer::is_keyword(const str identifier) {
    map_macro(check_keyword, keywords);
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

i64 Lexer::get_curr() {
    __FUMO_LINE_OFFSET__++;
    curr = file_stream.get();
    return curr;
}

[[nodiscard]] Token Lexer::parse_identifier() {
    str value = std::format("{}", char(curr));

    while (!identifier_ended()) {
        if (char next = get_curr(); std::isalnum(next) || next == '_') value += next;
        else
            PANIC(fmt_error("Source file is not valid ASCII."));
    }
    return Token {.type = is_keyword(value) ? TokenType::keyword : TokenType::identifier,
                  .value = value,
                  .line_number = __FUMO_LINE_NUM__,
                  .line_offset = __FUMO_LINE_OFFSET__};
}

[[nodiscard]] Token Lexer::parse_numeric_literal() {
    str value = std::format("{}", char(curr));
    Token token {.type = TokenType::integer};

    while (!identifier_ended() || file_stream.peek() == '.') {
        if (get_curr(); curr == '.' && std::isdigit(file_stream.peek())) {
            value += curr;
            token.type = TokenType::floating_point;
        } //
        else if (std::isdigit(curr))
            value += curr; // continue getting number
        else if (std::isalpha(curr)) {
            if (curr != 'f') // FIXME: technically only allowed at the end of floats
                PANIC(fmt_error("invalid digit '{}' in decimal constant.", curr));
        } else
            PANIC(fmt_error("Source file is not valid ASCII."));
    }

    if (token.type == TokenType::integer) token.value = std::stoi(value);
    else if (token.type == TokenType::floating_point)
        token.value = std::stof(value);
    else
        token.value = std::move(value);

    token.line_number = __FUMO_LINE_NUM__;
    token.line_offset = __FUMO_LINE_OFFSET__;
    return token;
}
