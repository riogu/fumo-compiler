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
enum struct ScopeKind {Local, Global};

struct SymbolTableStack {
    vec<std::map<str, ASTNode*>> symbols_to_nodes_stack {};
    // struct | enum | namespace 
    std::map<str, ASTNode*> named_scopes {};
    std::map<str, ASTNode*> function_names {}; 
    // all declarations are flattened and identifiers are changed to match them
    // for example:
    //   namespace foo  {struct bar {};} => struct "foo::bar"    {};
    //   struct    foo  {struct bar {};} => struct "foo{}::bar"  {};
    //   fn f() -> void {struct bar {};} => struct "foo()::bar"  {};
    //   struct    foo  {fn func()->void;} => fn   "foo{}::func"(this: foo*) -> void;
    //   namespace foo  {fn func()->void;} => fn   "foo::func"() -> void;
    // they are "global" but renamed internally
    str curr_scope_name = "";
    ScopeKind curr_scope_kind;
    auto push_to_scope(str identifier, ASTNode& node) {
        return symbols_to_nodes_stack.back().insert({curr_scope_name + identifier, &node});
    }
    auto push_named_scope(str identifier, ASTNode& node) {
        return named_scopes.insert({curr_scope_name + identifier, &node});
    }
    auto push_function(str identifier, ASTNode& node) {
        return function_names.insert({curr_scope_name + identifier, &node});
    }

};

struct Analyzer {
    Analyzer(const File& file) { file_stream << file.contents; }
    void semantic_analysis(ASTNode* file_scope);

  private:
    std::stringstream file_stream;

    SymbolTableStack symbol_tree {};

    void analyze(ASTNode& node);
    void report_binary_error(const ASTNode& node, const BinaryExpr& bin);

    [[nodiscard]] str push_scope(str name, ASTNode& node, ScopeKind scope_kind) {
        str prev_scope_name = symbol_tree.curr_scope_name;
        symbol_tree.curr_scope_name += name;
        symbol_tree.symbols_to_nodes_stack.push_back(std::map<str, ASTNode*> {});
        return prev_scope_name;
    }
    void pop_scope(str prev_scope_name, ASTNode& node) {
        symbol_tree.curr_scope_name = prev_scope_name;
        symbol_tree.symbols_to_nodes_stack.pop_back();
    }
    void add_to_scope(ASTNode& node);

    [[nodiscard]] ASTNode* find_function_decl(std::string_view var_name);
    [[nodiscard]] ASTNode* find_variable_decl(std::string_view var_name);

    void determine_type(ASTNode& node);
    [[nodiscard]] bool is_compatible_t(const Type& a, const Type& b);
    [[nodiscard]] bool is_arithmetic_t(const Type& a);

};
