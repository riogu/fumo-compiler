#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <print>

using i32 = int;

// TODO: write a script to run all the tests for the compiler (up to current stage)

auto main(int argc, char* argv[]) -> i32 {

    fs::path test = (argc > 1) ? argv[1] : "src/tests/testfile.c";

    std::vector<fs::path> file_paths {test};

    std::print("compiling file: {}...\n", test.string());

    Lexer lexer {};
    auto tokens = lexer.tokenize_file(file_paths.at(0));

    {
        std::string test = " int a = 123123;\n int b = 123123123;\n 1 + 2;";

        Lexer lexer {};
        auto tokens = lexer.tokenize_string(test);
        for (auto& token : tokens)
            std::cout << token.to_str() << "\t-> " << token.type_name() << "\n";

        Parser parser {};
        auto AST = parser.parse_tokens(tokens);
    }
}
