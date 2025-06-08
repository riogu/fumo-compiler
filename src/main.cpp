#include "lexer.hpp"
#include "token_definitions.hpp"
#include <iostream>
#define fn auto
using i32 = int;

fn main(int argc, char* argv[]) -> i32 {
    // std::string test = "write_a_c_compiler/stage_1/valid/return_2.c";
    std::string test = "src/testfile.c";
    auto tokens = Lexer::lex_file(test);
    for (auto token : tokens) std::cout << token_to_str(token.type);
}
