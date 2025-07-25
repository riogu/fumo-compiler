#pragma once
#include "parser/ast_node.hpp"
#include <llvm/IR/Function.h>
#include <map>

/*  
    turn identifiers into nodes that represent their respective variable declarations,
    or provide an error
    advanced type checking like generics, polymorphism will be done on a separate pass
*/

struct SymbolTableTree {
    vec<std::map<str, ASTNode*>> scope_to_nodes {};
    i64 depth = 0; // each scope moves forward 1 element in the vector
    // we clear the current depth when entering a new scope
    void open_scope();
    void close_scope();
    void insert_node(ASTNode* node);
    
    // struct | enum | namespace
    vec<ASTNode*> named_scopes {}; // these are kept separately 

    [[nodiscard]] ASTNode* find_node(std::string_view var_name) {
        PANIC("not implemented.");
    }
};

struct Analyzer {
    Analyzer(const File& file) { file_stream << file.contents; }
    void semantic_analysis(BlockScope& file_scope);

  private:
    std::stringstream file_stream;
    SymbolTableTree symbol_tree {};

    void analyze(ASTNode& node);
    void report_binary_error(const ASTNode& node, const BinaryExpr& bin) {
        switch (node.kind) {
          case NodeKind::add:       case NodeKind::sub:
          case NodeKind::multiply:  case NodeKind::divide:
          case NodeKind::equal:     case NodeKind::not_equal:
          case NodeKind::less_than: case NodeKind::less_equals:
              report_error(node.source_token,
                           "invalid operands to binary expression '{}' and '{}'.",
                            bin.lhs->type.name, bin.rhs->type.name);
          case NodeKind::assignment:
              report_error(node.source_token,
                           "assigning to '{}'from incompatible type '{}'.",
                            bin.lhs->type.name, bin.rhs->type.name);
            default:
                INTERNAL_PANIC("expected binary node for error, got '{}'.", node.kind_name());
        }
  }
};
