#include "lexer/lexer.hpp"
#include <iostream>

#define fn auto
using i32 = int;
using i64 = int64_t;

fn main(int argc, char* argv[]) -> i32 {
    // std::string test = "write_a_c_compiler/stage_1/valid/return_2.c";
    fs::path test = "src/testfile.c";

    Lexer lexer {};

    auto tokens = lexer.tokenize_file(test);

    if (!tokens) PANIC(tokens.error());

    for (auto token : tokens.value()) {
        std::cout << token_to_str(token) << "\t-> " << tokentype_name(token.type) << "\n";
        // std::cout << token_to_str(token) << " (" << tokentype_name(token.type) << ") | ";
        // std::cout << token_to_str(token);
    }
}
