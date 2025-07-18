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


    // Current task list:
    // - add functions 
    // - make symbol table. each scope has its own symbols.
    //   maybe we can provide errors in the parser itself for missmatching symbols (like redefinitions or lack of declaration)
    //   > the type solving should be deffered for later
    // 2-make small sample llvm project to test the library
    // 3-add llvm to my project starting from smaller parts


}
