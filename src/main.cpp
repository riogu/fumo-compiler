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
    // for(auto& token: tokens) std::print("{} and ", token.to_str());

    Parser parser {};
    parser.file_stream << file.contents;

    const auto AST = parser.parse_tokens(tokens);
    for (auto& node : AST) std::print("node found:\n  {}\n", node->to_str());

    // Codegen codegen {};
    // codegen.codegen(AST);

    // Plan:
    // 1-add functions and local variables to parser (maybe also solve a symbol table while making the AST)
    //      consider adding types
    // 2-make small sample llvm project to test the library
    // 3-add llvm to my project starting from smaller parts


}
