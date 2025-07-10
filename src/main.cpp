#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include <cpptrace/from_current.hpp>
#include <print>

using i32 = int;

// TODO: write a script to run all the tests for the compiler (up to current stage)

auto main(int argc, char* argv[]) -> i32 {

    Lexer lexer {};

    std::string test;
    if (argc > 1) test = argv[1];
    // auto [tokens, file] = lexer.tokenize_file(test);

    auto [tokens, file] = lexer.tokenize_string("test01", test);
    std::print("\ncompiling file: {}...", file.path_name.string());
    // for (auto& tkn : tokens) std::print("{}\t-> {}\n", tkn.to_str(), tkn.type_name());

    Parser parser {};
    parser.file_stream << file.contents;

    auto AST = parser.parse_tokens(tokens);
    std::print(
        "\n------------------------------------------------\n{}\n------------------------------------------------\n",
        file.contents);
    for (auto& node : AST) std::print("node found:\n  {}\n", node->to_str());
}
