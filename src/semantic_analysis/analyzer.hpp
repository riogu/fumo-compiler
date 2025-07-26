#pragma once
#include "parser/ast_node.hpp"
#include <llvm/IR/Function.h>
#include <map>

/*  
    turn identifiers into nodes that represent their respective variable declarations,
    or provide an error
    also performs basic type checking
*/

struct SymbolTableTree {
    vec<std::map<str, ASTNode*>> scoped_symbols_to_nodes {};
    // we clear the current depth when entering a new scope
    i64 depth = 0; // each scope moves forward 1 element in the vector
    // struct | enum | namespace
    vec<ASTNode*> named_scopes {}; // these are kept separately 

};

struct Analyzer {
    Analyzer(const File& file) { file_stream << file.contents; }
    void semantic_analysis(BlockScope& file_scope);

  private:
    std::stringstream file_stream;
    SymbolTableTree symbol_tree {};

    void analyze(ASTNode& node);
    void report_binary_error(const ASTNode& node, const BinaryExpr& bin);

    void open_scope()                                           { INTERNAL_PANIC("not implemented."); }
    void close_scope()                                          { INTERNAL_PANIC("not implemented."); }
    void add_node(ASTNode* node)                                { INTERNAL_PANIC("not implemented."); }
    void add_namespace(ASTNode* node)                           { INTERNAL_PANIC("not implemented."); }
    [[nodiscard]] ASTNode* find_node(std::string_view var_name) { INTERNAL_PANIC("not implemented."); }
};
