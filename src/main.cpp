#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "codegen/llvm_codegen.hpp"
#include "semantic_analysis/analyzer.hpp"


auto main(int argc, char** argv) -> int {
    std::string test;

    if (argc > 1) test = argv[1];

    Lexer lexer {};
    auto [tokens, file] = lexer.tokenize_string("fumo_module", test);
    // for(auto& token: tokens) std::cerr <<  token.to_str() + " | ";
    // std::cerr << "\n";

    Parser parser {file};
    auto file_root_node = parser.parse_tokens(tokens);
    // match(*file_root_node) {
    //     holds(const NamespaceDecl&, scope) for (auto& node : scope.nodes) std::cerr << "node found:\n  " + node->to_str() + "\n";
    //     _default {}
    // } std::cerr << "\n";

    Analyzer analyzer {file};
    analyzer.semantic_analysis(file_root_node);

    match(*file_root_node) {
        holds(const NamespaceDecl&, scope) for (auto& node : scope.nodes) std::cerr << "node found:\n  " + node->to_str() + "\n";
        _default {}
    } std::cerr << "\n";

    // Codegen codegen {file};
    // codegen.codegen(file_root_node);
    // std::cerr << "\ncodegen:\n" + codegen.llvm_ir_to_str() + "\n";
}
