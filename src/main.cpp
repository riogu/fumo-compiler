#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <cpptrace/from_current.hpp>
#include <iostream>

using i32 = int;

auto main(int argc, char* argv[]) -> i32 {

    fs::path test = (argc > 1) ? argv[1] : "src/tests/testfile.c";

    // TODO: write a script to run all the tests for the compiler (up to current stage)

    if (Lexer lexer; auto tokens = lexer.tokenize_file(test)) {
        for (auto& token : tokens.value())
            std::cout << token.to_str() << "\t-> " << token.type_name() << "\n";

        if (Parser parser {}; auto AST = parser.parse_tokens(tokens.value())) {
            // do stuff with the created AST
        } else
            PANIC(AST.error());

    } else
        PANIC(tokens.error());
}
