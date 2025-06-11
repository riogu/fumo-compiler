#include "lexer/lexer.hpp"
#include "lexer/token_definitions.hpp"
#include <iostream>
#include <optional>

#define fn auto
using i32 = int;
using i64 = int64_t;

fn main(int argc, char* argv[]) -> i32 {
    // std::string test = "write_a_c_compiler/stage_1/valid/return_2.c";
    fs::path test = "src/tests/test-symbol";

    Lexer lexer {};

    auto tokens = lexer.tokenize_file(test);

    if (!tokens) PANIC(tokens.error());

    for (auto token : tokens.value()) {
        std::cout << token.to_str() << "\t-> " << token.type_name() << "\n";
        // std::cout << token_to_str(token) << " (" << tokentype_name(token.type) << ") | ";
        // std::cout << token_to_str(token);
    }
}
