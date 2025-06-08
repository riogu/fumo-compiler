#include "lexer.hpp"
#include "token_definitions.hpp"
#include <iostream>
#define fn auto
using i32 = int;
using i64 = int64_t;

fn main(int argc, char* argv[]) -> i32 {
    // std::string test = "write_a_c_compiler/stage_1/valid/return_2.c";
    std::string test = "src/testfile.c";

    Lexer lexer {};
    auto tokens = lexer.tokenize_file(test);

    for (auto token : tokens) {
        // try {
        std::cout << token_to_str(token) << "\ttype: " << (int)token.type << "\n";
        // } catch (...) {
        //     PANIC(std::format("parsing error at line: {}.", lexer.__FUMO_LINE_NUM__));
        // }
    }
}
