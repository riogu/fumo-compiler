#include "codegen/llvm_codegen.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"

auto main(int argc, char* argv[]) -> int {
    std::string test;

    if (argc > 1) test = argv[1];
    Lexer lexer {};
    auto [tokens, file] = lexer.tokenize_string("fumo_module", test);
    // for(auto& token: tokens) std::print("{} and ", token.to_str());

    Parser parser {};
    parser.file_stream << file.contents;

    auto AST = parser.parse_tokens(tokens);
    for (auto& node : AST) std::cerr << "node found:\n  " + node.to_str() + "\n";

    Codegen codegen {file.path_name};
    codegen.codegen(AST);
    std::cerr << codegen.llvm_ir_to_str();
}
