#pragma once
#include <llvm/IR/Function.h>
#include "base_definitions/ast_node.hpp"
#include <map>
/*  
    turn identifiers into nodes that represent their respective variable declarations,
    or provide an error
    also performs basic type checking
*/

// works just like a stack of scopes
// we push a std::map when entering a new scope
// we pop the current std::map when leaving a scope
enum struct ScopeKind { Global, TypeBody, CompoundStatement };

struct SymbolTableStack {
    // these are for having local structs/functions/variables and then removing them
    vec<std::map<str, ASTNode*>> variable_env_stack {};
    vec<std::map<str, ASTNode*>> type_env_stack {};
    vec<std::map<str, ASTNode*>> function_env_stack {};
    // struct | enum | namespace
    std::map<str, ASTNode*> type_names {};
    std::map<str, ASTNode*> namespace_names {};
    std::map<str, ASTNode*> function_names {};
    std::map<str, ASTNode*> global_var_names {};
    // all declarations are flattened and identifiers are changed to match them
    // for example:
    //
    //   struct    foo  { struct bar {};     } => struct "foo::bar"    {};
    //   struct    foo  { fn func() -> void; } => fn     "foo{}::func"(this: foo*) -> void;
    //
    //   namespace foo  { struct bar {};     } => struct "foo::bar"    {};
    //   namespace foo  { fn func() -> void; } => fn     "foo::func"() -> void;
    //
    //   fn f() -> void { struct bar {};     } => struct "foo()::bar"  {};
    //   fn f() -> void { let bar: i32;      } => let    "foo()::bar": i32;
    //
    // they are "global" but renamed internally
    str curr_scope_name = "";
    ScopeKind curr_scope_kind;
    auto push_variable(str identifier, ASTNode& node) {
        switch (curr_scope_kind) {
            case ScopeKind::Global:
                return global_var_names.insert({curr_scope_name + identifier, &node});
            case ScopeKind::TypeBody:
                return variable_env_stack.back().insert({curr_scope_name + identifier, &node});
            case ScopeKind::CompoundStatement: 
                return variable_env_stack.back().insert({curr_scope_name + identifier, &node});
        }
    }
    auto push_type(str identifier, ASTNode& node) {
        switch (curr_scope_kind) {
            case ScopeKind::Global: 
                return type_names.insert({curr_scope_name + identifier, &node});
            case ScopeKind::TypeBody: 
                return type_names.insert({curr_scope_name + identifier, &node});
            case ScopeKind::CompoundStatement: 
                return type_names.insert({curr_scope_name + identifier, &node});
        }
    }
    auto push_namespace(str identifier, ASTNode& node) {
        return namespace_names.insert({curr_scope_name + identifier, &node});
    }
    auto push_function(str identifier, ASTNode& node) {
        switch (curr_scope_kind) {
            case ScopeKind::Global:
            case ScopeKind::TypeBody:
            case ScopeKind::CompoundStatement: 
        }
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
        symbol_tree.variable_env_stack.push_back(std::map<str, ASTNode*> {});
        return prev_scope_name;
    }
    void pop_scope(str prev_scope_name, ASTNode& node) {
        symbol_tree.curr_scope_name = prev_scope_name;
        symbol_tree.variable_env_stack.pop_back();
    }

    void add_to_scope(ASTNode& node);

    [[nodiscard]] ASTNode* find_declaration(ASTNode& node);

    [[nodiscard]] bool is_compatible_t(const Type& a, const Type& b);
    [[nodiscard]] bool is_arithmetic_t(const Type& a);

};
