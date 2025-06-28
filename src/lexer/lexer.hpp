#pragma once

#include "token_definitions.hpp"
#include <fstream>

#define fmt_error(...)                                                \
std::format("\n  | error in file '{}' at line {}:\n  | {}\n  |{}{}",  \
    __FUMO_FILE__,                                                    \
    __FUMO_LINE_NUM__,                                                \
    __FUMO_LINE__,                                                    \
    std::string(__FUMO_LINE_OFFSET__, ' ') + "^ ",                    \
    std::format(__VA_ARGS__)                                          \
    )

struct Lexer {
    i64 __FUMO_LINE_NUM__ = 1;
    i64 __FUMO_LINE_OFFSET__ = 0;
    std::string __FUMO_LINE__, __FUMO_FILE__;
    std::ifstream file_stream;
    i64 curr = 0;

    [[nodiscard]] Result<Vec<Token>, str> tokenize_file(const fs::path& _file_name);

  private:
    [[nodiscard]] Token parse_numeric_literal();
    [[nodiscard]] Token parse_identifier();
    [[nodiscard]] bool is_keyword(const str identifier);
    [[nodiscard]] str peek_line();
    [[nodiscard]] bool identifier_ended();
    i64 get_curr();
};

inline static int e = 123;
