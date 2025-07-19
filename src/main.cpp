#include "codegen/llvm_codegen.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

// TODO: write a script to run all the tests for the compiler (up to current stage)
auto main(int argc, char* argv[]) -> int {
    std::string test;

    if (argc > 1) test = argv[1];
    Lexer lexer {};
    auto [tokens, file] = lexer.tokenize_string("test01", test);
    // for(auto& token: tokens) std::print("{} and ", token.to_str());

    Parser parser {};
    parser.file_stream << file.contents;

    const auto AST = parser.parse_tokens(tokens);
    for (auto& node : AST) std::cerr << "node found:\n  " + node->to_str() + "\n";

    // Codegen codegen {};
    // codegen.codegen(AST);
}
