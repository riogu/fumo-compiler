#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "semantic_analysis/analyzer.hpp"
#include "codegen/llvm_codegen.hpp"
#include <llvm/Support/ManagedStatic.h>

auto main(int argc, char** argv) -> int {
    str file_name = "fumo_module.fm";
    std::string test;

    if (argc > 1) test = argv[1];

    Lexer lexer {};
    // auto [tokens, file] = lexer.tokenize_string(file_name, test);
    auto [tokens, file] = lexer.tokenize_file(file_name);

    // for(auto& token: tokens) std::cerr <<  token.to_str() + " | ";
    // std::cerr << "\n";

    Parser parser {file};
    auto file_root_node = parser.parse_tokens(tokens);

    Analyzer analyzer {file};
    analyzer.semantic_analysis(file_root_node);

    // str temp = file.path_name.string();
    // file.path_name = fs::current_path().string();
    // file.path_name /= temp;
    // INTERNAL_PANIC("{}", file.path_name.string());
    
    Codegen codegen {file, analyzer.symbol_tree};
    codegen.codegen_file(file_root_node);
    std::cerr << "codegen:\n" + codegen.llvm_ir_to_str() + "\n";
    llvm::llvm_shutdown();
//
//
//     for (const auto& node : get<NamespaceDecl>(file_root_node).nodes)
//     std::cerr << "node found:\n  " + node->to_str() + "\n";
//     std::cerr << "\n";
}
