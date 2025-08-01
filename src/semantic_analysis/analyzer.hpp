#pragma once
#include <llvm/IR/Function.h>
#include "base_definitions/ast_node.hpp"
#include <map>
#include<ranges>
// works just like a stack of scopes
// we push a std::map when entering a new scope
// we pop the current std::map when leaving a scope

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
enum struct ScopeKind { Namespace, TypeBody, CompoundStatement };
struct Scope {
    str name;
    ScopeKind kind;
    str isolated_name;
};
struct SymbolTableStack {
    std::map<str, ASTNode*> local_variable_decls {};
    std::map<str, ASTNode*> global_variable_decls {};
    std::map<str, ASTNode*> type_decls {};
    std::map<str, ASTNode*> namespace_decls {};
    std::map<str, ASTNode*> function_decls {};
    vec<Scope> scope_stack {};

    str curr_scope_name = "";
    ScopeKind curr_scope_kind;

    void push_scope(str name, ScopeKind scope) {
        curr_scope_kind = scope;
        curr_scope_name += name;
        scope_stack.push_back({curr_scope_name, scope, name});
    }
    void pop_scope() {
        curr_scope_name.resize(curr_scope_name.size() - scope_stack.back().isolated_name.size());
        scope_stack.pop_back();
    }

    auto push_variable_decl(Identifier& identifier, ASTNode& node) {
        identifier.mangled_name = curr_scope_name + identifier.name;
        switch (curr_scope_kind) {
            case ScopeKind::Namespace:
                return global_variable_decls.insert({identifier.mangled_name, &node});
            case ScopeKind::TypeBody:
            case ScopeKind::CompoundStatement: 
                return local_variable_decls.insert({identifier.mangled_name, &node});
        }
    }
    auto push_type_decl(Identifier& identifier, ASTNode& node) {
        identifier.mangled_name = curr_scope_name + identifier.name;
        return type_decls.insert({identifier.mangled_name, &node});
    }
    auto push_namespace_decl(Identifier& identifier, ASTNode& node) {
        identifier.mangled_name = curr_scope_name + identifier.name;
        return namespace_decls.insert({identifier.mangled_name, &node});
    }
    auto push_function_decl(Identifier& identifier, ASTNode& node) {
        identifier.mangled_name = curr_scope_name + identifier.name;
        return function_decls.insert({identifier.mangled_name, &node});
    }

    [[nodiscard]] Opt<ASTNode*> find_declaration(Identifier& id);
};

struct Analyzer {
    Analyzer(const File& file) { file_stream << file.contents; }
    void semantic_analysis(ASTNode* file_scope);

    SymbolTableStack symbol_tree {};
  private:
    std::stringstream file_stream;


    void analyze(ASTNode& node);
    void report_binary_error(const ASTNode& node, const BinaryExpr& bin);

    void add_declaration(ASTNode& node);

    [[nodiscard]] bool is_compatible_t(const Type& a, const Type& b);
    [[nodiscard]] bool is_arithmetic_t(const Type& a);

};
