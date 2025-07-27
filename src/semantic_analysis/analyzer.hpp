#pragma once
#include "base_definitions/ast_node.hpp"
#include <llvm/IR/Function.h>
#include <map>
/*  
    turn identifiers into nodes that represent their respective variable declarations,
    or provide an error
    also performs basic type checking
*/

// works just like a stack of scopes
// we push a std::map when entering a new scope
// we pop the current std::map when leaving a scope
struct SymbolTableStack {
    // struct | enum | namespace
    std::map<str, ASTNode*> named_scopes {}; // these are kept separately
    vec<std::map<str, ASTNode*>> symbols_to_nodes {};
    // all declarations are flattened and identifiers are changed to match them
    // for example:
    //   namespace foo  {struct bar {};} => struct "foo::bar"    {};
    //   struct    foo  {struct bar {};} => struct "foo{}::bar"  {};
    //   fn f() -> void {struct bar {};} => struct "foo()::bar"  {};
    //   struct    foo  {fn func()->void;} =>  fn "foo{}::func"() -> void;
    // they are "global" but renamed internally
    // enums cant have other declarations inside them
    auto push_to_scope(str identifier, ASTNode& node)    { return symbols_to_nodes.back().insert({identifier, &node}); }
    auto push_named_scope(str identifier, ASTNode& node) { return named_scopes.insert({identifier, &node}); }
};

struct Analyzer {
    Analyzer(const File& file) { file_stream << file.contents; }
    void semantic_analysis(ASTNode* file_scope);

  private:
    std::stringstream file_stream;
    SymbolTableStack symbol_tree {};

    void analyze(ASTNode& node);
    void report_binary_error(const ASTNode& node, const BinaryExpr& bin);

    void open_scope()  { symbol_tree.symbols_to_nodes.push_back(std::map<str, ASTNode*>{}); }
    void close_scope() { symbol_tree.symbols_to_nodes.pop_back(); }

    void push_to_scope(ASTNode& node);
    void push_named_scope(ASTNode& node);

    [[nodiscard]] ASTNode* find_node(std::string_view var_name);
    [[nodiscard]] constexpr bool is_compatible_t(const Type& a, const Type& b);
    [[nodiscard]] constexpr bool is_arithmetic_t(const Type& a);

};
