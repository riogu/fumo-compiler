#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "codegen/llvm_codegen.hpp"
#include <print>

// TODO: write a script to run all the tests for the compiler (up to current stage)
auto main(int argc, char* argv[]) -> int {
    std::string test;

    if (argc > 1) test = argv[1];
    Lexer lexer {};
    // auto [tokens, file] = lexer.tokenize_file(test);
    auto [tokens, file] = lexer.tokenize_string("test01", test);

    Parser parser {};
    parser.file_stream << file.contents;

    const auto AST = parser.parse_tokens(tokens);
    for (auto& node : AST) std::print("node found:\n  {}\n", node->to_str());

    Codegen codegen {};
    codegen.codegen(AST);

}
