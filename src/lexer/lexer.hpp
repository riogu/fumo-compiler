#pragma once

#include "token_definitions.hpp"
#include <fstream>

#define fmt_error(...)                                              \
 fmt("\n\t| error in file '{}' at line {}:\n\t| {}\n\t|{}{}",       \
    __FUMO_FILE__,                                                  \
    __FUMO_LINE_NUM__,                                              \
    __FUMO_LINE__,                                                  \
    std::string(__FUMO_LINE_OFFSET__, ' ') + "^ ",                  \
    fmt(__VA_ARGS__)                                                \
    )

struct Lexer {
    i64 __FUMO_LINE_NUM__ = 1;
    i64 __FUMO_LINE_OFFSET__ = 0;
    Str __FUMO_LINE__, __FUMO_FILE__;
    i64 curr = 0;
    std::ifstream file_stream;

    [[nodiscard]] Result<Vec<Token>, Str> tokenize_file(const fs::path _file_name);

  private:
    [[nodiscard]] Result<Token, Str> parse_numeric_literal();
    [[nodiscard]] Result<Token, Str> parse_identifier();
    [[nodiscard]] Str peek_line();
    [[nodiscard]] bool is_keyword(const Str identifier);
    [[nodiscard]] bool identifier_ended();
    i64 get_curr();
};
