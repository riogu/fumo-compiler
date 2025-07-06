#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <print>

using i32 = int;

// TODO: write a script to run all the tests for the compiler (up to current stage)

auto main(int argc, char* argv[]) -> i32 {
    Lexer lexer {};

    std::string test;

    // test = (argc > 1) ? argv[1] : "src/tests/testfile.c";
    // auto [tokens, file] = lexer.tokenize_file(test);

    test = "gamer = 69420; gamer = 1231 + 21312 * 3213 / (1231230 + 2130 + 10);\n";
    auto [tokens, file] = lexer.tokenize_string("test01", test);

    std::print("compiling file: {}...\n", file.path_name.string());

    for (auto& tkn : tokens) std::print("{}\t-> {}\n", tkn.to_str(), tkn.type_name());

    Parser parser {};
    parser.file_stream << file.file_string;

    auto AST = parser.parse_tokens(tokens);
    std::print(
        "\n------------------------------------------------\n{}------------------------------------------------\n",
        file.file_string);
    for (auto& node : AST) std::print("node found: '{}'\n", node->kind_name());
}
