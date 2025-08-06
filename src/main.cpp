#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic_analysis/analyzer.hpp"
#include "codegen/llvm_codegen.hpp"


auto main(int argc, char** argv) -> int {
    std::string test;

    if (argc > 1) test = argv[1];

    Lexer lexer {};
    auto [tokens, file] = lexer.tokenize_string("fumo_module", test);
    // for(auto& token: tokens) std::cerr <<  token.to_str() + " | ";
    // std::cerr << "\n";

    Parser parser {file};
    auto file_root_node = parser.parse_tokens(tokens);

    Analyzer analyzer {file};
    analyzer.semantic_analysis(file_root_node);


    for (const auto& node : get<NamespaceDecl>(file_root_node).nodes) std::cerr << "node found:\n  " + node->to_str() + "\n";
    std::cerr << "\n";

    // Codegen codegen {file, analyzer.symbol_tree};
    // codegen.codegen_file(file_root_node);
    // std::cerr << "\ncodegen:\n" + codegen.llvm_ir_to_str() + "\n";
}
