#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "codegen/llvm_codegen.hpp"
#include "semantic_analysis/analyzer.hpp"

auto main(int argc, char* argv[]) -> int {
    std::string test;

    if (argc > 1) test = argv[1];

    Lexer lexer {};
    auto [tokens, file] = lexer.tokenize_string("fumo_module", test);
    // for(auto& token: tokens) std::print("{} and ", token.to_str());

    Parser parser {file.contents};
    auto file_scope = parser.parse_tokens(tokens);
    for (auto& node : file_scope.nodes) std::cerr << "node found:\n  " + node->to_str() + "\n";

    Analyzer analyzer {};
    analyzer.semantic_analysis(file_scope);

    Codegen codegen {file};
    codegen.codegen(file_scope);
    // std::cerr << "\ncodegen:\n" + codegen.llvm_ir_to_str() + "\n";

}
