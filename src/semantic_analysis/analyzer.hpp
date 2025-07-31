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
struct SymbolTableStack {
    // these are for having local structs/functions/variables and then removing them
    std::map<str, ASTNode*> local_variable_decls {};
    std::map<str, ASTNode*> type_decls {};
    std::map<str, ASTNode*> namespace_decls {};
    std::map<str, ASTNode*> function_decls {};
    std::map<str, ASTNode*> global_variable_decls {};
    vec<str> scope_name_stack = {""};
    str curr_scope_name = "";
    ScopeKind curr_scope_kind;

    void push_scope(str name, ScopeKind scope) {
        curr_scope_kind = scope;
        scope_name_stack.push_back(name);
        curr_scope_name += name;
        // for (std::string_view name : scope_name_stack | std::views::reverse) {
        // }
    }
    void pop_scope() {
        curr_scope_name.resize(curr_scope_name.size() - scope_name_stack.back().size());
        scope_name_stack.pop_back();
    }

    auto push_variable_decl(str identifier, ASTNode& node) {
        switch (curr_scope_kind) {
            case ScopeKind::Namespace:
                return global_variable_decls.insert({curr_scope_name + identifier, &node});
            case ScopeKind::TypeBody:
            case ScopeKind::CompoundStatement: 
                return local_variable_decls.insert({curr_scope_name + identifier, &node});
        }
    }
    auto push_type_decl(str identifier, ASTNode& node) {
        return type_decls.insert({curr_scope_name + identifier, &node});
    }
    auto push_namespace_decl(str identifier, ASTNode& node) {
        return namespace_decls.insert({curr_scope_name + identifier, &node});
    }
    auto push_function_decl(str identifier, ASTNode& node) {
        return function_decls.insert({curr_scope_name + identifier, &node});
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

    void add_declaration(ASTNode& node);

    [[nodiscard]] Opt<ASTNode*> find_declaration(Identifier& id);

    [[nodiscard]] bool is_compatible_t(const Type& a, const Type& b);
    [[nodiscard]] bool is_arithmetic_t(const Type& a);

};
